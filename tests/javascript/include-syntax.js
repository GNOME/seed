#!../../src/seed

testsuite = imports.testsuite

try
{
    Seed.include("syntax-test-noasserts.js")
    testsuite.unreachable()
}
catch(e)
{
    testsuite.assert(e instanceof SyntaxError)
}

testsuite.checkAsserts(1)

