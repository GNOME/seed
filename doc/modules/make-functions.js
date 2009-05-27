#!/usr/bin/env seed
Seed.include(Seed.argv[2]);

function output_param (param){
    printf("<varlistentry><term><parameter>%s</parameter></term>"+
		"<listitem><simpara>%s</simpara></listitem></varlistentry>",
		param.name,
		param.description);
		
}

function output_function (func){
    var start = printf("<refsect2 id=\"%s\" role=\"function\">"+
			    "<title>%s</title>"+
			    "<para>%s</para>",
			    func.id, func.title, func.description);
    
    if (func.params || func.returns)
	print("<variablelist role=\"params\">");
    if (func.params != null){
	for (var i = 0; i < func.params.length; i++){
	    output_param (func.params[i]);
	}
    }
    if (func.returns != null){
	printf("<varlistentry><term><emphasis>Returns</emphasis></term>"+
		    "<listitem><simpara>%s</simpara></listitem></varlistentry>",
		    func.returns);
    }
    if (func.params || func.returns)
	print("</variablelist>");
    print("</refsect2>");
    
}

print("<para>");
for (var i = 0; i < funcs.length; i++){
    output_function (funcs[i]);
}
print("</para>");
