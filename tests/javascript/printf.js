#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:string\nstring\ninteger 4\nfloat 0\.333333\nmore 0\.333333333\nless 0\.33
// STDERR:
Seed.printf("string");
Seed.printf("%s", "string");
Seed.printf("%s %d", "integer", 2+2);
Seed.printf("%s %f", "float", 1/3);
Seed.printf("%s %.9f", "more", 1/3);
Seed.printf("%s %.2f", "less", 1/3);