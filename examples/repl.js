#!/usr/bin/env seed

readline = imports.readline;

bind_cr = function(){
    try {
	Seed.check_syntax(readline.buffer());
	readline.rl_done();
    }
    catch (e){
    }
    Seed.print("");
}

readline.bind('\n', bind_cr);
readline.bind('\r', bind_cr);

while(1){
    try{
	item = readline.readline("> ");
	Seed.print(eval(item));
    }
    catch(e){
	Seed.print(e.name + " " + e.message);
    }
}
