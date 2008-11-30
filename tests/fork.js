#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:[AB]\n[AB]
// STDERR:

var a = Seed.fork();

if(a)
{
	Seed.print("A");
}
else
{
	Seed.print("B");
}
