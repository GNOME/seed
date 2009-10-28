#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:ONE\n\[null\]\nTWO\n\[null\]\nTHREE
// STDERR:\n\*\* \(seed:[0-9]+\): CRITICAL \*\*: Line 1 in \[undefined\]: SyntaxError Parse error

print("ONE");
print(Seed.check_syntax("5+5;"));
print("TWO");
print(Seed.check_syntax("asdfasdf.jsdf()"));
print("THREE");
print(Seed.check_syntax("one[)"));
