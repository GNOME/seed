#!/usr/bin/env seed
Seed.include(Seed.argv[2]);

function output_param (param){
    print("<varlistentry><term><parameter>" + param.name +
			"</parameter></term>"  + "<listitem><simpara>" +
			param.descripton + "</simpara></listitem></varlistentry>");
		
}

function output_function (func){
    var start = print("<refsect2 id=\"" + func.id + "\" role=\"function\">" +
			    "<title>" + func.title + "</title>" +
			    "<para>" + func.description + "</para>");
    
    if (func.params || func.returns)
	print("<variablelist role=\"params\">");
    if (func.params != null){
	for (var i = 0; i < func.params.length; i++){
	    output_param (func.params[i]);
	}
    }
    if (func.returns != null){
	print("<varlistentry><term><emphasis>Returns</emphasis></term>" +
		    "<listitem><simpara>" + func.returns +
			"</simpara></listitem></varlistentry>");
    }
    if (func.params || func.returns)
	print("</variablelist>");
    print("</refsect2>");
    
}

print("<para>");
for (var i = 0; i < funcs.length; i++)
{
    output_function (funcs[i]);
}
print("</para>");

