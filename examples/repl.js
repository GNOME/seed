#!/usr/bin/env seed

readline = imports.readline;
sandbox = imports.sandbox;

var lastLastLength = '-1';

context = new sandbox.Context();
context.add_globals();

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

bind_tab = function(){
    os.write(1, "\t");
}

readline.bind('\n', bind_cr);
readline.bind('\r', bind_cr);
readline.bind('\t', bind_tab);

while(1){
    try{
	item = readline.readline("> ");
	Seed.print(context.eval(item));
    }
    catch(e){
	Seed.print(e.name + " " + e.message);
    }
}
