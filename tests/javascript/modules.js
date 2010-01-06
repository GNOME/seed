#!../../src/seed

testsuite = imports.testsuite

input = Seed.spawn("../../src/seed modules-noasserts.js");

testsuite.assert(input.stdout == "Hello Seed Module World\n")

