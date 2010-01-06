#!../../src/seed

testsuite = imports.testsuite

try
{
    Seed.check_syntax("5+5;")
}
catch(e)
{
    testsuite.unreachable("5+5; is not a syntax error")
}

try
{
    Seed.check_syntax("asdfasdf.jsdf()")
}
catch(e)
{
    testsuite.unreachable("asdfasdf.jsdf() is not a syntax error")
}

try
{
    Seed.check_syntax("one[)")
    testsuite.unreachable("one[) should be a syntax error")
}
catch(e)
{
    testsuite.assert(e instanceof SyntaxError)
}

