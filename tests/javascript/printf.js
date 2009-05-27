#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:string\nstring\ninteger 4\nfloat 0\.333333\nmore 0\.333333333\nless 0\.33
// STDERR:

printf("string");
printf("%s", "string");
printf("%s %d", "integer", 2+2);
printf("%s %f", "float", 1/3);
printf("%s %.9f", "more", 1/3);
printf("%s %.2f", "less", 1/3);
