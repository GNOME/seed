#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:[AB]\n[AB]
// STDERR:
os = imports.os;
var a = os.fork();

if(a)
{
	print("A");
}
else
{
	print("B");
}
