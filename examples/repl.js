#!/usr/bin/env seed

readline = imports.readline;
sandbox = imports.sandbox;
os = imports.os;

var lastLastLength = '-1';

context = new sandbox.Context();
context.add_globals();

bind_cr = function(){
    var buffer = readline.buffer();
    if (buffer.length == lastLastLength)
	readline.done();
    try {
	Seed.check_syntax(buffer);
	readline.done();
    }
    catch (e){
	os.write(1, "\n..");
	lastLastLength = buffer.length;
	return;
    }
    os.write(1, "\n");
    lastLastLength = buffer.length;
}

readline.bind('\n', bind_cr);
readline.bind('\r', bind_cr);
readline.bind('\t', function(){
    readline.insert("\t");
});

//var re = /[^=<>*-^/]=[^=<>*-^/]\(*\s*(new\s*)?[^:punct:]|'|"+\)*$/

while(1){
    try{
	item = readline.readline("> ");
	result = context.eval(item);
//	if (!re.exec(item) && (result != undefined))
	if (result != undefined)
	    print(result)

    }
    catch(e){
	print(e.name + " " + e.message);
    }
}
