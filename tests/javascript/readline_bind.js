#!/usr/bin/env seed
// Returns: 0
// STDIN:a 2+2
// STDOUT:Got here!\n4.000000
// STDERR:

Seed.import_namespace("readline");
readline.bind("a", function () { Seed.print("Got here!"); });
Seed.print(eval(readline.readline("")));
