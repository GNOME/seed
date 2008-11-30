#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:ONE\n\[null\]\nTWO\n\[null\]\nTHREE
// STDERR:\n\*\* \(seed:[0-9]+\): CRITICAL \*\*: SyntaxError\. Parse error in \[null\] at line 1

Seed.print("ONE");
Seed.print(Seed.check_syntax("5+5;"));
Seed.print("TWO");
Seed.print(Seed.check_syntax("asdfasdf.jsdf()"));
Seed.print("THREE");
Seed.print(Seed.check_syntax("one[)"));
