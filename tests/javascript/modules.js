#!/usr/bin/env seed

testsuite = imports.testsuite

input = Seed.spawn(Seed.argv[0] + " modules-noasserts.js");

testsuite.assert(input.stdout == "Hello Seed Module World\n")

