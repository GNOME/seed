#!/usr/bin/env seed

sqlite = imports.sqlite;

d = new sqlite.Database(Seed.argv[2]);

d.exec("create table t1 (t1key INTEGER PRIMARY KEY,data TEXT,num double,timeEnter DATE);");
d.exec("insert into t1 (data,num) values ('This is sample data',3);");
d.exec("insert into t1 (data,num) values ('More sample data',6);");
d.exec("insert into t1 (data,num) values ('And a little more',9);");

d.exec("select * from t1", function(results){Seed.print(JSON.stringify(results))});

d.close();
