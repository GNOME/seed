#!/usr/bin/env seed

while(1)
{
	try
	{
		Seed.print(eval(Seed.readline("> ")));
	}
	catch(e)
	{
		Seed.print(e.name + " " + e.message);
	}
}
