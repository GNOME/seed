#!/usr/bin/env seed

testsuite = imports.testsuite
Gio = imports.gi.Gio
sqlite = imports.sqlite
JSON = imports.JSON

try
{
    Gio.file_new_for_path("/tmp/.seed_test.db")["delete"]()
}
catch(e)
{
    // We don't care if we fail to delete the (probably nonexistent) file...
}

d = new sqlite.Database("/tmp/.seed_test.db")

d.exec("create table t1 (t1key INTEGER PRIMARY KEY,data TEXT,num double,timeEnter DATE)")
d.exec("insert into t1 (data,num) values ('This is sample data',3)")
d.exec("insert into t1 (data,num) values ('More sample data',6)")
d.exec("insert into t1 (data,num) values ('And a little more',9)")

d.exec("select * from t1 where num = 6", function(results) {
    testsuite.assert(results.t1key == "2")
    testsuite.assert(results.data == "More sample data")
    testsuite.assert(results.num == 6.0)
})

d.close()

testsuite.checkAsserts(3)
