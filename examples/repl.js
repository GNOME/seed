#!/usr/bin/env seed

readline = imports.readline;

while(1){
    try{
	item = readline.readline("> ");
	Seed.print(eval(item));
    }
    catch(e){
	Seed.print(e.name + " " + e.message);
    }
}
