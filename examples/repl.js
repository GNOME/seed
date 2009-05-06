#!/usr/bin/env seed

readline = imports.readline;

var eoq = 0;
var bc = 0;

bind_cr = function(){
    if (bc == 0){
	readline.rl_done();
	eoq = 0;
    }
    Seed.print("");
}

bind_eoq = function(){
    eoq = 1;
    Seed.print(";");
}

bind_lb = function(){
    bc++;
    Seed.print("{");
}
bind_rb = function(){
    bc--;
    Seed.print("}");
}

readline.bind('\n', bind_cr);
readline.bind('\r', bind_cr);
readline.bind('{', bind_lb);
readline.bind('}', bind_rb);

while(1){
    try{
	item = readline.readline("> ");
	Seed.print(eval(item));
    }
    catch(e){
	Seed.print(e.name + " " + e.message);
    }
}
