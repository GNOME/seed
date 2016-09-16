#!/usr/bin/env seed

// when running make check, argv is actually path/to/lt-seed or path/to/seed

imports.testsuite.assert(Seed.argv[0] == "seed" || Seed.argv[0].match(/\/lt-seed$/) || Seed.argv[0].match(/\/seed$/))
