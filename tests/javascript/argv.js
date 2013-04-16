#!/usr/bin/env seed

// when running make check, argv is actually path/to/lt-seed

imports.testsuite.assert(Seed.argv[0] == "seed" || Seed.argv[0].match(/\/lt-seed$/))
