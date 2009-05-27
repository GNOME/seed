#!/usr/bin/env seed
xml = imports.libxml;
doc = xml.parseFile("./sample.xml");
ctx = doc.xpathNewContext();

results = ctx.xpathEval("//story/body/headline");
print("Headline: " + results.value[0].content);
