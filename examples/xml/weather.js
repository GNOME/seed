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

ctx.xpathRegisterNs("yweather", "http://xml.weather.yahoo.com/ns/rss/1.0");
Seed.print(ctx.xpathEval("//yweather:condition/@temp").value[0].content);

