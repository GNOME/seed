#!/usr/bin/env seed
xml = imports.libxml

doc = xml.parseFile("sample.xml");

root = doc.root;
storyinfos = root.getElementsByTagName("storyinfo");
for (var i = 0; i < storyinfos.length; i++){
    var info = storyinfos[i];
    var keyword = info.getElementsByTagName("keyword")[0];
    
    printf ("Story info Keyword (example: %s): %s", 
		 keyword.getAttribute("example"),
		 keyword.content);
		 
}
