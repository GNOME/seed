prototype = Seed.prototype(Gio.FileInputStream);

prototype.get_contents = function()
{
	var stream = Gio.DataInputStream._new(this);
	var line = stream.read_until("", 0);
	return line;	
}

Gio.simple_write = function(file, name)
{
	var file = Gio.file_new_for_path(file);
	var fstream = file.replace();
	var dstream = Gio.DataOutputStream._new(fstream);
	
	dstream.put_string(name);
	fstream.close();
}

Gio.simple_read = function(name)
{
	var file = Gio.file_new_for_path(name);
	var fstream = file.read();
	var dstream = Gio.DataInputStream._new(fstream);
	var line = dstream.read_until("", 0);
	fstream.close();
	return line;
}
