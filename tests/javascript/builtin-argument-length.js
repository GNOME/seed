#!/usr/bin/env seed

testsuite = imports.testsuite

function correctArgs(fs, a)
{
    try
    {
        eval(fs).apply(this, a)
    }
    catch(e)
    {
        if(e.name == "ArgumentError")
            testsuite.unreachable(fs)
    }
}

function wrongArgs(fs, a)
{
    try
    {
        eval(fs).apply(this, a)
        testsuite.unreachable(fs)
    }
    catch(e)
    {
        
    }
}

wrongArgs("Seed.spawn", [])
correctArgs("Seed.spawn", ["asdfasdfasdf"])

wrongArgs("Seed.include", [])
wrongArgs("Seed.include", [1])
correctArgs("Seed.include", [""])
wrongArgs("Seed.include", [1,2])
wrongArgs("Seed.include", ["fail.js","another.js"])

wrongArgs("print", [])
wrongArgs("print", [1,2])
wrongArgs("print", ["asdf",2])
wrongArgs("print", [1,2,3])

wrongArgs("Seed.introspect", [])
correctArgs("Seed.introspect", [Seed])
wrongArgs("Seed.introspect", [Seed, 5.23])

wrongArgs("Seed.check_syntax", [])
correctArgs("Seed.check_syntax", ["asdfasdf"])
wrongArgs("Seed.check_syntax", ["5+5", "asdf"])

