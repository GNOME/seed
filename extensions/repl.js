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

function complete_globals(text)
{
    var glob = context.eval("(function(){ return this; })();");  // global object
    var properties = Object.getOwnPropertyNames(glob);
    completions = properties;
    completions = completions.filter(function(s) { return s.indexOf(text) == 0; });
}

function complete_class_field(klass, klass_part, field_part)
{
    var properties = Object.getOwnPropertyNames(klass);
    completions = properties;
    completions = completions.filter(function(s) { return s.indexOf(field_part) == 0; });
    completions = completions.map(function(s){ return klass_part + '.' + s });
}

function complete_object_field(obj, obj_part, field_part)
{
    var properties = Object.getOwnPropertyNames(obj);
//    var methods = Object.getOwnPropertyNames(obj.constructor.prototype);
    var methods = Object.getOwnPropertyNames(Object.getPrototypeOf(obj));  // better (eg imports.os)
    completions = properties.concat(methods);
    completions = completions.filter(function(s) { return s.indexOf(field_part) == 0; });
    completions = completions.map(function(s){ return obj_part + '.' + s });    
}

function complete_parse(text)
{
    try
    {
	if (text.indexOf('.') == -1)
	{
	    complete_globals(text);
	    return;
	}
	
	var a = text.split('.');    
	var field_part = a.pop();  // first letters of field to complete
	var obj_part = a.join('.');
	var obj = context.eval(obj_part);
	if (typeof obj == "object")
	    complete_object_field(obj, obj_part, field_part);
	if (typeof obj == "function")
	    complete_class_field(obj, obj_part, field_part);
    }
    catch (e)
    {  }
}

var completions = [];

function complete(text, state)
{
//    os.write(1, "text: '" + text + "'\n");

    if (!state) // new word to complete
    {
	completions = [];
	complete_parse(text);
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
