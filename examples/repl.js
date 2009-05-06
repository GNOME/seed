#!/usr/bin/env seed

readline = imports.readline;

var lastLastLength = '-1';

bind_cr = function(){
    var buffer = readline.buffer();
    if (buffer.length == lastLastLength)
	readline.rl_done();
    try {
	Seed.check_syntax(buffer);
	readline.rl_done();
    }
    catch (e){
	if (buffer[buffer.length] == '\n' || buffer[buffer.length] == '\r')
	    readline.rl_done();
    }
    Seed.print("");
    lastLastLength = buffer.length;
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
