#!/usr/bin/env seed
Seed.include(Seed.argv[2]);

function output_param (param){
    Seed.printf("<varlistentry><term><parameter>%s</parameter></term>"+
		"<listitem><simpara>%s</simpara></listitem></varlistentry>",
		param.name,
		param.description);
		
}

function output_function (func){
    var start = Seed.printf("<refsect2 id=\"%s\" role=\"function\">"+
			    "<title>%s</title>"+
			    "<para>%s</para>",
			    func.id, func.title, func.description);
    
    if (func.params || func.returns)
	Seed.print("<variablelist role=\"params\">");
    if (func.params != null){
	for (var i = 0; i < func.params.length; i++){
	    output_param (func.params[i]);
	}
    }
    if (func.returns != null){
	Seed.printf("<varlistentry><term><emphasis>Returns</emphasis></term>"+
		    "<listitem><simpara>%s</simpara></listitem></varlistentry>",
		    func.returns);
    }
    if (func.params || func.returns)
	Seed.print("</variablelist>");
    Seed.print("</refsect2>");
    
}

Seed.print("<para>");
for (var i = 0; i < funcs.length; i++){
    output_function (funcs[i]);
}
Seed.print("</para>");
