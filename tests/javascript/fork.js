#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:[AB]\n[AB]
// STDERR:

var a = Seed.fork();

if(a)
{
	print("A");
}
else
{
	print("B");
}
