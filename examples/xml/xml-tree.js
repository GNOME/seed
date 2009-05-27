#!/usr/bin/env seed
xml = imports.libxml;

function print_element (element){
    var child = element.children;

    while (child){
	if (child.type == "element"){
	    print(child.name + ": " + child.content);
	    print_element (child);
	}
	child = child.next;
    }
}

doc = xml.parseFile("sample.xml");
print_element (doc.root);
