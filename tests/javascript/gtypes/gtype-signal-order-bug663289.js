#!/usr/bin/env seed

testsuite = imports.testsuite
GObject = imports.gi.GObject

TestObject = new GType({
    parent: GObject.Object.type,
    name: "TestObject",
    signals: [{name: "more_args",
               parameters: [GObject.TYPE_INT,
                            GObject.TYPE_STRING]},
              {name: "less_args",
               return_type: GObject.TYPE_BOOLEAN}]
})

t = new TestObject()

t.signal.less_args.connect(function(object)
{
   return true
})

testsuite.assert(t.signal.less_args.emit())

testsuite.checkAsserts(1)
