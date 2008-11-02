prototype = Seed.prototype(Gio.FileInputStream);

prototype.get_contents = function()
{
	stream = Gio.DataInputStream._new(input);
	line = stream.read_until("", 0);
	return line;	
}
