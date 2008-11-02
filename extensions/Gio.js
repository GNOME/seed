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