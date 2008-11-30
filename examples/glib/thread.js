#!/usr/local/bin/seed
Seed.import_namespace("GLib");

count = 0;
one = true;
function threadf()
{
	while (count < 100)
	{
		while (one){}
		Seed.print("thread 2 " + count);
		count++;
		one = true;
	}
}

thread = GLib.thread_create_full(threadf, null, 0, true);
while(count < 100)
{
	while (!one){}
	Seed.print("thread 1 " + count);
	count ++;
	one = false;
}

GLib.thread_join(thread);

