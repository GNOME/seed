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

function complete_obj_get_obj(text)
{
    try
    {
	var a = text.split('.');    a.pop();
	var obj_str = a.join('.');
	var obj = context.eval(obj_str);
	return (typeof obj == "object" ? obj : null);
    }
    catch (e)
    { return null; }
}

var completions = [];

function complete(text, state)
{
//    os.write(1, "text: '" + text + "'\n");

    if (!state) // new word to complete
    {
	var obj = complete_obj_get_obj(text);
	if (!obj)
	    return null;
	var a = text.split('.');
	var field_part = a.pop();  // first letters of field to complete
	var obj_part = a.join('.');
	var klass = obj.constructor;
	var properties = Object.getOwnPropertyNames(obj);
	var methods = Object.getOwnPropertyNames(klass.prototype);
	completions = properties.concat(methods);
	completions = completions.filter(function(s) { return s.indexOf(field_part) == 0; });
	completions = completions.map(function(s){ return obj_part + '.' + s });
    }

    if (state == completions.length)
	return null;
    return completions[state];
}

readline.bind('\n', bind_cr);
readline.bind('\r', bind_cr);

readline.completion_entry(complete);
//readline.bind('\t', function(){ readline.insert("\t"); });


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
