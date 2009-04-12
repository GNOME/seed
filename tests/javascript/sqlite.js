#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\{"t1key":"1","data":"This is sample data","num":"3\.0","timeEnter":""\}\n\{"t1key":"2","data":"More sample data","num":"6\.0","timeEnter":""\}\n\{"t1key":"3","data":"And a little more","num":"9\.0","timeEnter":""\}
// STDERR:
Gio = imports.gi.Gio;
sqlite = imports.sqlite;

try
{
	Gio.file_new_for_path("/tmp/.seed_test.db")["delete"]();
}
catch(e)
{
}

d = new sqlite.Database("/tmp/.seed_test.db");

d.exec("create table t1 (t1key INTEGER PRIMARY KEY,data TEXT,num double,timeEnter DATE);");
d.exec("insert into t1 (data,num) values ('This is sample data',3);");
d.exec("insert into t1 (data,num) values ('More sample data',6);");
d.exec("insert into t1 (data,num) values ('And a little more',9);");

d.exec("select * from t1", function(results){Seed.print(JSON.stringify(results));});

d.close();
