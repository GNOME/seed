#!/usr/bin/env seed

// The way this test stood, it didn't make any sense. It still doesn't, but
// at least now it's not fragile based on Gdk version, and will still be
// triggered by the only flaw this test has ever caught...

testsuite = imports.testsuite
Gdk = imports.gi.Gdk

maxindent = 0
propcount = 0

function enum_structlike(indent, e)
{
    if(indent > maxindent)
        maxindent = indent
    
	for (prop in e)
	{
	    propcount++
	    
		try
		{
			if (e[prop] && e[prop].toString() && 
				(e[prop].toString().search("struct") > 0 ||
				e[prop].toString().search("union") > 0))
				enum_structlike(indent + 1, e[prop])
		}
		catch(e)
		{
			
		}
	}
}

e = new Gdk.Event()

enum_structlike(0, e)

testsuite.assert(maxindent >= 2)
testsuite.assert(propcount >= 50)
