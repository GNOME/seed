Gio = imports.gi.Gio;

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
}).apply();
