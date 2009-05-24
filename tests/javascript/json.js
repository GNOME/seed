#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:
JSON = imports.JSON;

a = {test: 3,
     hello: "Goodbye",
     more: {a: "1", b: 2, c:"d"}};
json = JSON.stringify(a);
object = JSON.parse(json);
json2 = JSON.stringify(object);
if (json != json2)
    Seed.print("Failure");
       
