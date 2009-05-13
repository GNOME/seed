#!/usr/local/bin/seed
xml = imports.libxml;
doc = xml.parseFile("./sample.xml");
ctx = doc.xpathNewContext();

results = ctx.xpathEval("//story/body/headline");
Seed.print("Headline: " + results.value[0].content);
