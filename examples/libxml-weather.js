#!/usr/bin/env seed

Soup = imports.gi.Soup;
libxml = imports.libxml;
readline = imports.readline;

var zipcode = "05446";

var session = new Soup.SessionSync();

// Soup.URI is a struct.
var uri = new Soup.URI.c_new("http://weather.yahooapis.com/forecastrss?p=" + zipcode);
var request = new Soup.Message({method:"GET", uri:uri});
var status = session.send_message(request);

var data = request.response_body.data;
var doc = libxml.parseString(data);
var ctx = doc.xpathNewContext()

Seed.print(data);
Seed.print(ctx.xpathEval("//condition").value[0].content);

