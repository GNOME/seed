// Copyright 2008 litl, LLC. All Rights Reserved.

const Lang = imports.lang;
const StringUtil = imports.stringUtil;
const ArrayUtil = imports.arrayUtil;

// Parameters for acquire_name, keep in sync with
// enum in util/dbus.h
const SINGLE_INSTANCE = 0;
const MANY_INSTANCES = 1;

const NO_START_IF_NOT_FOUND = false;
const START_IF_NOT_FOUND    = true;

// Merge stuff defined in native code
Lang.copyProperties(imports.dbusNative, this);

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
    let replyFunc;

    /* Note: "this" in here is the module, "obj" is the proxy object
     */

    if (ifaceName == null)
        ifaceName = obj._dbusInterface;

    /* Convert arg_array to a *real* array */
    arg_array = Array.prototype.slice.call(arg_array);

    /* The default replyFunc only logs the responses */
    replyFunc = _logReply;

    let expectedNumberArgs = this.signatureLength(inSignature);

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

function _logReply(result, exc) {
    if (result != null) {
        log("Ignored reply to dbus method: " + result.toSource());
    }
    if (exc != null) {
        log("Ignored exception from dbus method: " + exc.toString());
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
    let argArray = [emitter];
    let length = argsFromDBus.length;
    for (let i = 0; i < length; ++i) {
        argArray.push(argsFromDBus[i]);
    }
    callback.apply(null, argArray);
}

function _connect(signalName, callback) {
    if (!this._signalNames || !(signalName in this._signalNames))
        throw new Error("Signal " + signalName + " not defined in this object");

    let me = this;
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
    let argArray = [].splice.call(arguments, 0);
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
    let argArray = [].splice.call(arguments, 0);
    // prepend iface the property is on
    argArray.splice(0, 0, this._dbusInterface);

    _proxyInvoker(this, "org.freedesktop.DBus.Properties",
                  "Set",
                  "",
                  "ssv",
                  -1,
                  argArray);
}

function _getAllRemote() {
    // convert arguments to regular array
    let argArray = [].splice.call(arguments, 0);
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
        let methods = iface.methods;

        for (let i = 0; i < methods.length; ++i) {
            let method = methods[i];

            if (!('name' in method))
                throw new Error("Method definition must have a name");

            /* To ease transition let's create two methods: Foo and FooRemote.
             * Foo will just log a warning so we can catch people using the
             * old naming system. FooRemote is the actual proxy method.
             */
            let methodName = method.name + "Remote";
            proto[methodName] = _makeProxyMethod(method);
            proto[method.name] = function() {
                log("PROXY-ERROR: " + method.name + " called, you should be using " +
                    methodName + " instead");
            };
        }
    }

    if ('signals' in iface) {
        let signals = iface.signals;
        let signalNames = {};

        for (let i = 0; i < signals.length; i++) {
            let signal = signals[i];

            if (!('name' in signal))
                throw new Error("Signal definition must have a name");

            let name = signal.name;

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
this._busProto.proxifyObject = function(obj, busName, path) {
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
let _addExports = function(node, path, object) {
    if (path == '') {
        if ('-impl-' in node)
            throw new Error("Object already registered for path.");
        node['-impl-'] = object;
    } else {
        let idx = path.indexOf('/');
        let head = (idx >= 0) ? path.slice(0, idx) : path;
        let tail = (idx >= 0) ? path.slice(idx+1) : '';
        if (!(head in node))
            node[head] = {};
        _addExports(node[head], tail, object);
    }
};

// remove any implementation from exports at the given path.
let _removeExportsPath = function(node, path) {
    if (path == '') {
        delete node['-impl-'];
    } else {
        let idx = path.indexOf('/');
        let head = (idx >= 0) ? path.slice(0, idx) : path;
        let tail = (idx >= 0) ? path.slice(idx+1) : '';
        // recursively delete next component
        _removeExportsPath(node[head], tail);
        // are we empty now?  if so, clean us up.
        if ([x for (x in node[head])].length == 0) {
            delete node[head];
        }
    }
};


// export the object at the specified object path
this._busProto.exportObject = function(path, object) {
    if (path.slice(0,1) != '/')
        throw new Error("Bad path!  Must start with /");
    // keep session and system paths separate, just in case we register on both
    let pathProp = '_dbusPaths' + this._dbusBusType;
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
    let pathProp = '_dbusPaths' + this._dbusBusType;
    if (!(pathProp in object))
        return; // already or never registered.
    let dbusPaths = object[pathProp].split(':');
    for (let i = 0; i < dbusPaths.length; ++i) {
        let path = dbusPaths[i];
        _removeExportsPath(this.exports, path.slice(1));
    }
    delete object[pathProp];
};


// given a "this" with _dbusInterfaces, return a dict from property
// names to property signatures. Used as value of _dbus_signatures
// when passing around a dict of properties.
function _getPropertySignatures(ifaceName) {
    let iface = this._dbusInterfaces[ifaceName];
    if (!iface)
        throw new Error("Object has no interface " + ifaceName);
    if (!('_dbusPropertySignaturesCache' in iface)) {
        let signatures = {};
        if ('properties' in iface) {
            let properties = iface.properties;
            for (let i = 0; i < properties.length; ++i) {
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
    let c = s.charAt(0);
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
function _parseDBusSigs(sig) {
    while (sig.length > 0) {
        let one = [];
        sig = _eatSCT(one, sig);
        yield one.join('');
    }
}

// given a "this" with _dbusInterfaces, returns DBus Introspection
// format XML giving interfaces and methods implemented.
function _getInterfaceXML() {
    let result = '';
    // iterate through defined interfaces
    let ifaces = ('_dbusInterfaces' in this) ? this._dbusInterfaces : {};
    ifaces = [i for each (i in ifaces)]; // convert to array
    // add introspectable and properties
    ifaces.push(Introspectable);
    ifaces.push(Properties);

    for (let i = 0; i < ifaces.length; i++) {
        let iface = ifaces[i];
        result += StringUtil.sprintf('  <interface name="%s">\n', iface.name);
        // describe methods.
        let methods = ('methods' in iface) ? iface.methods : [];
        for (let j = 0; j < methods.length; j++) {
            let method = methods[j];
            result += StringUtil.sprintf(
                '    <method name="%s">\n', method.name);
            for each (let sig in _parseDBusSigs(method.inSignature)) {
                result += StringUtil.sprintf(
                    '      <arg type="%s" direction="in"/>\n', sig);
            }
            for each (let sig in _parseDBusSigs(method.outSignature)) {
                result += StringUtil.sprintf(
                    '      <arg type="%s" direction="out"/>\n', sig);
            }
            result += '    </method>\n';
        }
        // describe signals
        let signals = ('signals' in iface) ? iface.signals : [];
        for (let j = 0; j < signals.length; j++) {
            let signal = signals[j];
            result += StringUtil.sprintf(
                '    <signal name="%s">\n', signal.name);
            for each (let sig in _parseDBusSigs(signal.inSignature)) {
                result += StringUtil.sprintf(
                    '      <arg type="%s"/>\n', sig);
            }
            result += '    </signal>\n';
        }
        // describe properties
        let properties = ('properties' in iface) ? iface.properties : [];
        for (let j = 0; j < properties.length; j++) {
            let property = properties[j];
            result += StringUtil.sprintf(
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
        let methods = iface.methods;

        for (let i = 0; i < methods.length; ++i) {
            let method = methods[i];

            if (!('name' in method))
                throw new Error("Method definition must have a name");

            if (!('outSignature' in method))
                method.outSignature = "a{sv}";

            let name = method.name;
            let asyncName = name +"Async";

            let missing = [];
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
        let properties = iface.properties;

        let missing = [];
        for (let i = 0; i < properties.length; ++i) {
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
        let signals = iface.signals;

        for (let i = 0; i < signals.length; ++i) {
            let signal = signals[i];
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
