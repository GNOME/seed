#!/usr/bin/env seed

multiprocessing = imports.multiprocessing;
Gtk = imports.gi.Gtk;
os = imports.os;
JSON = imports.JSON;

pipes = new multiprocessing.Pipe();
child_pid = os.fork();

if (child_pid == 0)
{
	mine = pipes[0];
	mine.add_watch(1,
				   function()
				   {
					   var message = {type: "PING",
									  data: "Hello!"};
					   mine.write(JSON.stringify(message));
					   return true;
				   });
	Gtk.main();
}

mine = pipes[1];
mine.write("Ping");
message = JSON.parse(mine.read());
print("Parent Got: " + message.type + ": " + message.data);
