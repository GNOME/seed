#!/usr/bin/env seed
with (Seed)
{
	while(1)
	{
		try
		{
			item = readline("> ");
			print(eval(item));
		}
		catch(e)
		{
			print(e.name + " " + e.message);
		}
	}
}
