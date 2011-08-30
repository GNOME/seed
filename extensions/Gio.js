GLib = imports.gi.GLib;
Gio = imports.gi.Gio;
Lang = imports.lang;

(function()
{
    var prototype = Gio.FileInputStream.prototype;

    prototype.get_contents = function()
    {
	    var stream = new Gio.DataInputStream.c_new(this);
	    var line = stream.read_until("", 0);
	return line;
    }

    Gio.simple_write = function(file, name)
    {
	    var file = Gio.file_new_for_path(file);
	    var fstream = file.replace();
	    var dstream = new Gio.DataOutputStream.c_new(fstream);

	    dstream.put_string(name);
	    fstream.close();
    }

    Gio.simple_read = function(name)
    {
	    var file = Gio.file_new_for_path(name);

	    var fstream = file.read();
	    var dstream = new Gio.DataInputStream.c_new(fstream);
	    var line = dstream.read_until("", 0);

	    fstream.close();
	    return line;
    }

    Gio.DBusProxy.prototype._call = _call;
    Gio.DBusProxy.prototype._callAsync = _callAsync;
    Gio.DBusProxy.new = _simple_proxy_factory;
}).apply();

/* Handler for GDBusProxy::g_properties_change
 * Invoke the javascript handler, if any, for each changed property
 * Property change handler should be declared as on($propertyName)Changed
 * field of the proxy instance.
 */
function _dispatchPropertiesChanged (proxy,
				     changedPropsVariant,
				     invalidatedProps,
				     userData)
{
    var jsonRes = changedPropsVariant.toJSON();
    for (var changedProperty in jsonRes)
    {
	handlerName = 'on'+changedProperty+'Changed';
	if (proxy[handlerName])
	    proxy[handlerName] (jsonRes[changedProperty]);
    }
}

/* Handler for GDBusProxy::g_signal
 * Invoke the javascript handler, if any, registered for the incoming
 * signal.
 * Signal handler should be declared as on($signalName) field of the proxy
 * instance.
 */
function _dispatchSignal (proxy, senderName, signalName, parameters)
{
    handlerName = 'on'+signalName;
    if (!this[handlerName])
	return;

    var jsonRes = parameters.toJSON ();
    proxy[handlerName] (jsonRes);
}

/* Synchronous call of a remote DBus method
 */
function _call (method, inSignature, outSignature, args)
{
    var variantRes =
	this.call_sync (method,
			args ? GLib.Variant.new(inSignature, arg) : null,
			Gio.DBusCallFlags.NONE, -1);

    return  variantRes.toJSON ();
}

/* Asynchronous call of a remote DBus method
 * The callback function must take arguments with the same signature
 * as the return value of the invoked method.
 */
function _callAsync (method, inSignature, outSignature, callback, args)
{
    var variantRes = this.call (method,
				args || args != '' ? GLib.Variant.new(inSignature, args) : null,
				Gio.DBusCallFlags.NONE,
				-1,
				null,
				function (p, res) {
				    var variantRes = p.call_finish (res);
				    var jsonRes = variantRes.toJSON ();
				    if (callback)
					callback (jsonRes);
				},
				"");
}

/* Create a DBus proxy for the given service
 * Two Javascript methods are created for each introspected
 * DBus method name $methodName:
 * - proxy.$methodName: sync version
 * - proxy.$methodNameAsync: asyn version
 */
function _simple_proxy_factory (bus, name, object, iface) {
    var res = new Gio.DBusProxy.for_bus_sync (bus,
					      Gio.DBusProxyFlags.NONE,
					      null,
					      name,
					      object,
					      iface,
					      null);

    var introspectionProxy =
	new Gio.DBusProxy.for_bus_sync (bus,
					Gio.DBusProxyFlags.NONE,
					null,
					name,
					object,
					"org.freedesktop.DBus.Introspectable",
					null);

    var objectDescription = introspectionProxy._call ('Introspect',
						      null, 's', null);
    var parser = imports.libxml;
    var xmlDoc = parser.parseString (objectDescription);
    var objectDescription = xmlDoc.root.toJSON();
    var interfacesArray = objectDescription['interface'];

    var interfaces = {};
    for (var i = 0; i < interfacesArray.length; i++)
	interfaces[interfacesArray[i].name] = interfacesArray[i];

    var requestedInterface = interfaces[iface];

    for (var i = 0;i < requestedInterface.method.length; i++)
    {
	var methodDescription = requestedInterface.method[i];
	var name = methodDescription.name;
	var inSignature = _dbusInSignatureFromJSON (methodDescription);
	var outSignature = _dbusOutSignatureFromJSON (methodDescription);

	// capture name, inSignature, outSignature
	res[name] = function(name, inSignature, outSignature) {
	    return function () {
		var args = Array.prototype.slice.call (arguments, 0);
		return res._call (name, inSignature, outSignature, args)
	    }
	} (name, inSignature, outSignature);

	res[name+'Async'] = function (name, inSignature, outSignature) {
	    return function () {
		var callback;
		var args;
		if (typeof (arguments[0]) == 'function')
		{
		    callback = arguments[0];
		    args = Array.prototype.slice.call (arguments, 1);
		}
		else
		{
		    callback = this['on'+name+'Returned'];
		    args = Array.prototype.slice.call (arguments, 0);
		}

		res._callAsync(name,
			       inSignature, outSignature, callback, args);
	    }
	} (name, inSignature, outSignature);
    }

    res.signal.connect ("g_properties_changed",
			Lang.bind(res, _dispatchPropertiesChanged));

    res.signal.connect ("g_signal",  Lang.bind(res, _dispatchSignal));

    return res;
}

/// Utils ///

/* Generate the DBus/Variant signature from a json description of the method
 */
function _dbusSignatureFromJSON (jsonMethodDescription, dir)
{
    if (!jsonMethodDescription.arg)
	return;

    args = jsonMethodDescription.arg;
    var res = '';
    for (var i = 0; i < args.length; i++)
	if (args[i].direction == dir)
	    res += args[i].type;
    return '('+res+')';
}
function _dbusOutSignatureFromJSON (json)
{ return _dbusSignatureFromJSON(json, 'out'); }
function _dbusInSignatureFromJSON (json)
{ return _dbusSignatureFromJSON(json, 'in'); }
