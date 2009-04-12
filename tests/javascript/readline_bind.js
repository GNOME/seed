#!/usr/bin/env seed
// Returns: 0
// STDIN:a 2+2
// STDOUT:Got here!\n4.000000
// STDERR:

const readline = imports.readline;
readline.bind("a", function () { Seed.print("Got here!"); });
Seed.print(eval(readline.readline("")));
