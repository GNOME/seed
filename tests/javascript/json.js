#!../../src/seed

testsuite = imports.testsuite;
JSON = imports.JSON;

a = {test: 3,
     hello: "Goodbye",
     more: {a: "1", b: 2, c:"d"}}

json = JSON.stringify(a)
object = JSON.parse(json)
json2 = JSON.stringify(object)

testsuite.assert(json == json2)
testsuite.assert(object.test == 3)
testsuite.assert(object.hello == "Goodbye")
testsuite.assert(object.more.a == "1")
testsuite.assert(object.more.b == 2)
testsuite.assert(object.more.c == "d")

