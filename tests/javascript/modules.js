#!/usr/bin/env seed

testsuite = imports.testsuite

input = Seed.spawn("/usr/bin/env seed modules-noasserts.js");

testsuite.assert(input.stdout == "Hello Seed Module World\n")

