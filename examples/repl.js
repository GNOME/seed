#!/usr/bin/env seed
history = [];
with (Seed)
{
	while(1)
	{
		try
		{
			item = readline("> ");
			history.push(item);
			print(eval(item));
		}
		catch(e)
		{
			print(e.name + " " + e.message);
		}
	}
}
