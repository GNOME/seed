assertCount = 0

function assert(a, err)
{
    assertCount++
    
    if(!a)
    {
        if(err)
            print("Assertion " + assertCount + " failed: " + err + ".")
        else
            print("Assertion " + assertCount + " failed.")
        
        Seed.quit(assertCount)
    }
}

function unreachable(err)
{
    if(err)
        print("Unreachable code reached: " + err.toString() + ".")
    else
        print("Unreachable code reached.")
    
    Seed.quit(1)
}

function checkAsserts(n)
{
    if(assertCount != n)
        assert(0, "Some assertions failed to run")
}
