#!/usr/bin/env seed
// Returns: 0
// STDIN:a 2+2
// STDOUT:Got here!\n4
// STDERR:

readline = imports.readline;
readline.bind("a", function () { print("Got here!"); });
print(eval(readline.readline("")));
