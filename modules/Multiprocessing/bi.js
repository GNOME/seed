#!/usr/bin/env seed

Seed.import_namespace("Multiprocessing");
Seed.import_namespace("Gtk");

pipes = new Multiprocessing.Pipe();
child_pid = Seed.fork();

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
Seed.print("Parent Got: " + message.type + ": " + message.data);
