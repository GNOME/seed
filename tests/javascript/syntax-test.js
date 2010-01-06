#!/usr/bin/env seed

testsuite = imports.testsuite

try
{
    eval("new = 3")
    testsuite.unreachable()
}
catch(e)
{
    testsuite.assert(e instanceof SyntaxError)
}

testsuite.checkAsserts(1)
