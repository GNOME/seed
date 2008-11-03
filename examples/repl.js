#!/usr/local/bin/seed
Seed.include("json2.js");

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
