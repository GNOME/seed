#!/usr/local/bin/seed
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
					   Seed.print("Child Got: " + mine.read());
					   mine.write("Pong");
					   return true;
				   });
	Gtk.main();									
}
mine = pipes[1];
mine.write("Ping");
Seed.print("Parent Got: " + mine.read());