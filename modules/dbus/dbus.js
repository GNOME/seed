// Copyright 2008 litl, LLC. All Rights Reserved.
log = print
Lang = imports.lang;
//const StringUtil = imports.stringUtil;
//const ArrayUtil = imports.arrayUtil;

// Parameters for acquire_name, keep in sync with
// enum in util/dbus.h
const SINGLE_INSTANCE = 0;
const MANY_INSTANCES = 1;

const NO_START_IF_NOT_FOUND = false;
const START_IF_NOT_FOUND    = true;

// Merge stuff defined in native code
dbusnative = imports.dbusnative;
Lang.copyProperties(dbusnative, this);

function propsToArray(obj) {
    var a = new Array();
    for (i in ifaces)
	a.push(i);
    return a;
}


var Introspectable = {
    name: 'org.freedesktop.DBus.Introspectable',
    methods: [
        // Introspectable: return an XML description of the object.
        // Out Args:
        //   xml_data: XML description of the object.
        { name: 'Introspect', inSignature: '', outSignature: 's' }
    ],
    signals: [],
    properties: []
};

var Properties = {
    name: 'org.freedesktop.DBus.Properties',
    methods: [
        { name: 'Get', inSignature: 'ss', outSignature: 'v' },
        { name: 'Set', inSignature: 'ssv', outSignature: '' },
        { name: 'GetAll', inSignature: 's', outSignature: 'a{sv}' }
    ],
    signals: [],
    properties: []
};

function _proxyInvoker(obj, ifaceName, methodName, outSignature, inSignature, timeout, arg_array) {
    var replyFunc;

    /* Note: "this" in here is the module, "obj" is the proxy object
     */

    if (ifaceName == null)
        ifaceName = obj._dbusInterface;

    /* Convert arg_array to a *real* array */
    arg_array = Array.prototype.slice.call(arg_array);

    /* The default replyFunc only logs the responses */
    replyFunc = _logReply;

    var expectedNumberArgs = this.signatureLength(inSignature);

    if (arg_array.length < expectedNumberArgs) {
        throw new Error("Not enough arguments passed for method: " + methodName +
                       ". Expected " + expectedNumberArgs + ", got " + arg_array.length);
    } else if (arg_array.length == expectedNumberArgs + 1) {
        /* If there's one more than the expected number
         * of argument we asume the last one is the reply
         * function
         */
        replyFunc = arg_array.pop();
    } else if (arg_array.length != expectedNumberArgs) {
        throw new Error("Too many arguments passed for method: " + methodName +
                       ". Maximum is " + expectedNumberArgs + " + one callback");
    }

    /* Auto-start on method calls is too unpredictable; in particular if
     * something crashes we want to cleanly restart it, not have it
     * come back next time someone tries to use it.
     */
    obj._dbusBus.call_async(obj._dbusBusName,
                            obj._dbusPath,
                            ifaceName,
                            methodName,
                            outSignature,
                            inSignature,
                            NO_START_IF_NOT_FOUND,
                            timeout,
                            arg_array,
                            replyFunc);
}

function _proxyInvokerSync(obj, ifaceName, methodName, outSignature, inSignature, timeout, arg_array) {
    if (ifaceName == null)
        ifaceName = obj._dbusInterface;

    /* Convert arg_array to a *real* array */
    arg_array = Array.prototype.slice.call(arg_array);

    var expectedNumberArgs = this.signatureLength(inSignature);

    if (arg_array.length < expectedNumberArgs) {
        throw new Error("Not enough arguments passed for method: " + methodName +
                       ". Expected " + expectedNumberArgs + ", got " + arg_array.length);
    } else if (arg_array.length > expectedNumberArgs) {
        throw new Error("Too many arguments passed for method: " + methodName +
                       ". Maximum is " + expectedNumberArgs);
    }

    /* Auto-start on method calls is too unpredictable; in particular if
     * something crashes we want to cleanly restart it, not have it
     * come back next time someone tries to use it.
     */
    return obj._dbusBus.call(obj._dbusBusName,
			     obj._dbusPath,
			     ifaceName,
			     methodName,
			     outSignature,
			     inSignature,
			     NO_START_IF_NOT_FOUND,
			     arg_array);

}

function _logReply(result, exc) {
    if (result != null) {
        log("Ignored reply to dbus method: " + result.toSource());
    }
    if (exc != null) {
        log("Ignored exception from dbus method: " + exc.message);
    }
}

function _makeProxyMethod(member) {
    return function() {
        /* JSON methods are the default */
        if (!("outSignature" in member))
            member.outSignature = "a{sv}";

        if (!("inSignature" in member))
            member.inSignature = "a{sv}";

        if (!("timeout" in member))
            member.timeout = -1;

        _proxyInvoker(this, null, member.name,
                      member.outSignature, member.inSignature,
                      member.timeout, arguments);
    };
}

function _makeProxyMethodSync(member) {
    return function() {
        /* JSON methods are the default */
        if (!("outSignature" in member))
            member.outSignature = "a{sv}";

        if (!("inSignature" in member))
            member.inSignature = "a{sv}";

        if (!("timeout" in member))
            member.timeout = -1;

        return _proxyInvokerSync(this, null, member.name,
				 member.outSignature, member.inSignature,
				 member.timeout, arguments);
    };
}

// stub we insert in all proxy prototypes
function _getBusName() {
    return this._dbusBusName;
}

// stub we insert in all proxy prototypes
function _getPath() {
    return this._dbusPath;
}

// stub we insert in all proxy prototypes
function _getBus() {
    return this._dbusBus;
}

// stub we insert in all proxy prototypes
function _getInterface() {
    return this._dbusInterface;
}

function _invokeSignalWatchCallback(callback, emitter, argsFromDBus) {
    var argArray = [emitter];
    var length = argsFromDBus.length;
    for (var i = 0; i < length; ++i) {
        argArray.push(argsFromDBus[i]);
    }
    callback.apply(null, argArray);
}

function _connect(signalName, callback) {
    if (!this._signalNames || !(signalName in this._signalNames))
        throw new Error("Signal " + signalName + " not defined in this object");

    var me = this;
    return this._dbusBus.watch_signal(this._dbusBusName,
                                      this._dbusPath,
                                      this._dbusInterface,
                                      signalName,
                                      function() {
                                          _invokeSignalWatchCallback(callback,
                                                                     me,
                                                                     arguments);
                                      });
}

function _disconnectByID(ID) {
    this._dbusBus.unwatch_signal_by_id(ID);
}

function _getRemote(propName) {
    // convert arguments to regular array
    var argArray = [].splice.call(arguments, 0);
    // prepend iface the property is on
    argArray.splice(0, 0, this._dbusInterface);

    _proxyInvoker(this, "org.freedesktop.DBus.Properties",
                  "Get",
                  "v",
                  "ss",
                  -1,
                  argArray);
}

function _setRemote(propName, value) {
    // convert arguments to regular array
    var argArray = [].splice.call(arguments, 0);
    // prepend iface the property is on
    argArray.splice(0, 0, this._dbusInterface);

    _proxyInvoker(this, "org.freedesktop.DBus.Properties",
                  "Set",
                  "",
                  "ssv",
                  -1,
                  argArray);
}

function _getRemoteSync(propName) {
    // convert arguments to regular array
    var argArray = [].splice.call(arguments, 0);
    // prepend iface the property is on
    argArray.splice(0, 0, this._dbusInterface);

    return _proxyInvokerSync(this, "org.freedesktop.DBus.Properties",
			     "Get",
			     "v",
			     "ss",
			     -1,
			     argArray);
}

function _setRemoteSync(propName, value) {
    // convert arguments to regular array
    var argArray = [].splice.call(arguments, 0);
    // prepend iface the property is on
    argArray.splice(0, 0, this._dbusInterface);

    _proxyInvokerSync(this, "org.freedesktop.DBus.Properties",
                  "Set",
                  "",
                  "ssv",
                  -1,
                  argArray);
}

function _getAllRemote() {
    // convert arguments to regular array
    var argArray = [].splice.call(arguments, 0);
    // prepend iface the properties are on
    argArray.splice(0, 0, this._dbusInterface);

    _proxyInvoker(this, "org.freedesktop.DBus.Properties",
                  "GetAll",
                  "a{sv}",
                  "s",
                  -1,
                 argArray);
}

// adds remote members to a prototype. The instances using
// the prototype must be proxified with proxifyObject, too.
// Note that the bus (system vs. session) is per-instance not
// per-prototype so the bus should not be involved in this
// function.
// The "iface" arg is a special object that might have properties
// for 'name', 'methods', and 'signals'
function proxifyPrototype(proto, iface) {
    if ('name' in iface)
        proto._dbusInterface = iface.name;
    else
        proto._dbusInterface = null;

    if ('methods' in iface) {
        var methods = iface.methods;

        for (var i = 0; i < methods.length; ++i) {
            var method = methods[i];

            if (!('name' in method))
                throw new Error("Method definition must have a name");

            /* To ease transition let's create two methods: Foo and FooRemote.
             * Foo will just log a warning so we can catch people using the
             * old naming system. FooRemote is the actual proxy method.
             */
            var methodName = method.name + "Remote";
            proto[methodName] = _makeProxyMethod(method);
	    proto[methodName+"Sync"] = _makeProxyMethodSync(method);
            proto[method.name] = function() {
                log("PROXY-ERROR: " + method.name + " called, you should be using " +
                    methodName + " instead");
            };
        }
    }

    if ('signals' in iface) {
        var signals = iface.signals;
        var signalNames = {};

        for (var i = 0; i < signals.length; i++) {
            var signal = signals[i];

            if (!('name' in signal))
                throw new Error("Signal definition must have a name");

            var name = signal.name;

            /* This is a bit silly, but we might want to add more information
             * later and it still beats an Array in checking if the object has
             * a given signal
             */
            signalNames[name] = name;
        }

        proto['_signalNames'] = signalNames;
        proto['connect'] = _connect;
        proto['disconnect'] = _disconnectByID;
    }

    // properties just adds GetRemote, SetRemote, and GetAllRemote
    // methods; using attributes would force synchronous API.
    if ('properties' in iface &&
        iface.properties.length > 0) {
        proto['GetRemote'] = _getRemote;
        proto['SetRemote'] = _setRemote;
        proto['GetRemoteSync'] = _getRemoteSync;
        proto['SetRemoteSync'] = _setRemoteSync;
        proto['GetAllRemote'] = _getAllRemote;
    }

    proto['getBusName'] = _getBusName;
    proto['getPath'] = _getPath;
    proto['getBus'] = _getBus;
    proto['getInterface'] = _getInterface;
}

// methods common to both session and system buses.
// add them to the common prototype of this.session and this.system.

// convert any object to a dbus proxy ... assumes its prototype is
// also proxified
this.system.proxifyObject = this.session.proxifyObject = function(obj, busName, path) {
    if (!busName) {
        throw new Error('missing bus name proxifying object');
    }
    if (!path) {
        throw new Error('missing path proxifying object');
    }
    if (path[0] != '/')
        throw new Error("Path doesn't look like a path: " + path);

    obj._dbusBus = this;
    obj._dbusBusName = busName;
    obj._dbusPath = path;
};

// add 'object' to the exports object at the given path
var _addExports = function(node, path, object) {
    if (path == '') {
        if ('-impl-' in node)
            throw new Error("Object already registered for path.");
        node['-impl-'] = object;
    } else {
        var idx = path.indexOf('/');
        var head = (idx >= 0) ? path.slice(0, idx) : path;
        var tail = (idx >= 0) ? path.slice(idx+1) : '';
        if (!(head in node))
            node[head] = {};
        _addExports(node[head], tail, object);
    }
};
// Fiiix
// remove any implementation from exports at the given path.
var _removeExportsPath = function(node, path) {
    if (path == '') {
        delete node['-impl-'];
    } else {
        var idx = path.indexOf('/');
        var head = (idx >= 0) ? path.slice(0, idx) : path;
        var tail = (idx >= 0) ? path.slice(idx+1) : '';
        // recursively delete next component
        _removeExportsPath(node[head], tail);
        // are we empty now?  if so, clean us up.
	// If we had destructuring assignment, this would be cool
        if (propsToArray(node[head]).length == 0) {
            delete node[head];
	}
    }
};

this._busProto = this.session.__proto__
// export the object at the specified object path
this._busProto.exportObject = function(path, object) {
    if (path.slice(0,1) != '/')
        throw new Error("Bad path!  Must start with /");
    // keep session and system paths separate, just in case we register on both
    var pathProp = '_dbusPaths' + this._dbusBusType;
    // optimize for the common one-path case by using a colon-delimited
    // string rather than an array.
    if (!(pathProp in object)) {
        object[pathProp] = path;
    } else {
        object[pathProp] += ':' + path;
    }
    _addExports(this.exports, path.slice(1), object);
};

// unregister this object from all its paths
this._busProto.unexportObject = function(object) {
    // keep session and system paths separate, just in case we register on both
    var pathProp = '_dbusPaths' + this._dbusBusType;
    if (!(pathProp in object))
        return; // already or never registered.
    var dbusPaths = object[pathProp].split(':');
    for (var i = 0; i < dbusPaths.length; ++i) {
        var path = dbusPaths[i];
        _removeExportsPath(this.exports, path.slice(1));
    }
    delete object[pathProp];
};


// given a "this" with _dbusInterfaces, return a dict from property
// names to property signatures. Used as value of _dbus_signatures
// when passing around a dict of properties.
function _getPropertySignatures(ifaceName) {
    var iface = this._dbusInterfaces[ifaceName];
    if (!iface)
        throw new Error("Object has no interface " + ifaceName);
    if (!('_dbusPropertySignaturesCache' in iface)) {
        var signatures = {};
        if ('properties' in iface) {
            var properties = iface.properties;
            for (var i = 0; i < properties.length; ++i) {
                signatures[properties[i].name] =
                    properties[i].signature;
            }
        }
        iface._dbusPropertySignaturesCache = signatures;
    }
    return iface._dbusPropertySignaturesCache;
}

// split a "single complete type" (SCT) from the rest of a dbus signature.
// Returns the tail, and the head is added to acc.
function _eatSCT(acc, s) {
    // eat the first character
    var c = s.charAt(0);
    s = s.slice(1);
    acc.push(c);
    // if this is a compound type, loop eating until we reach the close paren
    if (c=='(') {
        while (s.charAt(0) != ')') {
            s = _eatSCT(acc, s);
        }
        s = _eatSCT(acc, s); // the close paren
    } else if (c=='{') {
        s = _eatSCT(acc, s);
        s = _eatSCT(acc, s);
        if (s.charAt(0) != '}')
            throw new Error("Bad DICT_ENTRY signature!");
        s = _eatSCT(acc, s); // the close brace
    } else if (c=='a') {
        s = _eatSCT(acc, s); // element type
    }
    return s;
}

// parse signature string, generating a list of "single complete types"
//function _parseDBusSigs(sig) {
   // while (sig.length > 0) {
  //    var one = [];
    //    sig = _eatSCT(one, sig);
      //  yield one.join('');
//    }
//}

function _parseDBusSigs(sig){
    var sigs = [];
    while (sig.length > 0) {
	sig = _eatSCT(sigs, sig);
    }
    return sigs.join('');
}


// given a "this" with _dbusInterfaces, returns DBus Introspection
// format XML giving interfaces and methods implemented.
function _getInterfaceXML() {
    var result = '';
    // iterate through defined interfaces
    var ifaces = ('_dbusInterfaces' in this) ? this._dbusInterfaces : {};
    ifaces = propsToArray(ifaces);
    // add introspectable and properties
    ifaces.push(Introspectable);
    ifaces.push(Properties);

    for (var i = 0; i < ifaces.length; i++) {
        var iface = ifaces[i];
        result += Seed.sprintf('  <interface name="%s">\n', iface.name);
        // describe methods.
        var methods = ('methods' in iface) ? iface.methods : [];
        for (var j = 0; j < methods.length; j++) {
            var method = methods[j];
            result += Seed.sprintf(
                '    <method name="%s">\n', method.name);
            for (var sig in _parseDBusSigs(method.inSignature)) {
                result += Seed.sprintf(
                    '      <arg type="%s" direction="in"/>\n', sig);
            }
            for (var sig in _parseDBusSigs(method.outSignature)) {
                result += Seed.sprintf(
                    '      <arg type="%s" direction="out"/>\n', sig);
            }
            result += '    </method>\n';
        }
        // describe signals
        var signals = ('signals' in iface) ? iface.signals : [];
        for (var j = 0; j < signals.length; j++) {
            var signal = signals[j];
            result += Seed.sprintf(
                '    <signal name="%s">\n', signal.name);
            for (var sig in _parseDBusSigs(signal.inSignature)) {
                result += Seed.sprintf(
                    '      <arg type="%s"/>\n', sig);
            }
            result += '    </signal>\n';
        }
        // describe properties
        var properties = ('properties' in iface) ? iface.properties : [];
        for (var j = 0; j < properties.length; j++) {
            var property = properties[j];
            result += Seed.sprintf(
                '    <property name="%s" type="%s" access="%s"/>\n',
                property.name, property.signature, property.access);
        }
        // close <interface> tag
        result += '  </interface>\n';
    }
    return result;
}

// Verifies that a given object conforms to a given interface,
// and stores meta-info from the interface in the object as
// required.
// Note that an object can be exported on multiple buses, so conformExport()
// should not be specific to the bus (session vs. system)
function conformExport(object, iface) {

    if (!('_dbusInterfaces' in object)) {
        object._dbusInterfaces = {};
    } else if (iface.name in object._dbusInterfaces) {
        return;
    }

    object._dbusInterfaces[iface.name] = iface;

    object.getDBusPropertySignatures =
        _getPropertySignatures;

    if (!('getDBusInterfaceXML' in object))
        object.getDBusInterfaceXML = _getInterfaceXML;

    if ('methods' in iface) {
        var methods = iface.methods;

        for (var i = 0; i < methods.length; ++i) {
            var method = methods[i];

            if (!('name' in method))
                throw new Error("Method definition must have a name");

            if (!('outSignature' in method))
                method.outSignature = "a{sv}";

            var name = method.name;
            var asyncName = name +"Async";

            var missing = [];
            if (!(name in object) &&
                !(asyncName in object)) {
                missing.push(name);
            } else {
                if (asyncName in object)
                    object[asyncName].outSignature = method.outSignature;
                else
                    object[name].outSignature = method.outSignature;
            }

            if (missing.length > 0) {
                throw new Error("Exported object missing members " + missing.toSource());
            }
        }
    }

    if ('properties' in iface) {
        var properties = iface.properties;

        var missing = [];
        for (var i = 0; i < properties.length; ++i) {
            if (!(properties[i].name in object)) {
                missing += properties[i];
            }
        }

        if (missing.length > 0) {
            throw new Error("Exported object missing properties " + missing.toSource());
        }
    }

    // sanity-check signals
    if ('signals' in iface) {
        var signals = iface.signals;

        for (var i = 0; i < signals.length; ++i) {
            var signal = signals[i];
            if (signal.name.indexOf('-') >= 0)
                throw new Error("dbus signals cannot have a hyphen in them, use camelCase");
        }
    }
}

// DBusError object
// Used to return dbus error with given name and mesage from
// remote methods

function DBusError(dbusErrorName, errorMessage) {
    this.dbusErrorName = dbusErrorName;
    this.errorMessage = errorMessage;
}

DBusError.prototype = {
    toString: function() {
        return this.errorMessage;
    }
};

// Introspectable proxy
function IntrospectableProxy(bus, busName, path) {
    this._init(bus, busName, path);
}

IntrospectableProxy.prototype = {
    _init : function(bus, busName, path) {
        bus.proxifyObject(this, busName, path);
    }

};

proxifyPrototype(IntrospectableProxy.prototype,
                 Introspectable);
