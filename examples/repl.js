#!/usr/bin/env seed

with (Seed)
{
	while(1)
	{
		try
		{
			print(eval(readline("> ")));
		}
		catch(e)
		{
			print(e.name + " " + e.message);
		}
	}
}
