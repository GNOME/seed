#!/usr/bin/env seed

Soup = imports.gi.Soup;
libxml = imports.libxml;
readline = imports.readline;

var zipcode = "05446";

if(Seed.argv[2])
	zipcode = Seed.argv[2];

var session = new Soup.SessionSync();

// Soup.URI is a struct.
var uri = new Soup.URI.c_new("http://weather.yahooapis.com/forecastrss?p=" + zipcode);
var request = new Soup.Message({method:"GET", uri:uri});
var status = session.send_message(request);

var data = request.response_body.data;
var doc = libxml.parseString(data);
var ctx = doc.xpathNewContext();

ctx.xpathRegisterNs("yweather", "http://xml.weather.yahoo.com/ns/rss/1.0");

var title = ctx.xpathEval("//item/title").value[0].content;

var temp_unit = ctx.xpathEval("//yweather:units/@temperature").value[0].content;
var distance_unit = ctx.xpathEval("//yweather:units/@distance").value[0].content;
var pressure_unit = ctx.xpathEval("//yweather:units/@pressure").value[0].content;
var speed_unit = ctx.xpathEval("//yweather:units/@speed").value[0].content;

var temperature = ctx.xpathEval("//yweather:condition/@temp").value[0].content;
var wind_chill = ctx.xpathEval("//yweather:wind/@chill").value[0].content;
var wind_speed = ctx.xpathEval("//yweather:wind/@speed").value[0].content;
var wind_direction = ctx.xpathEval("//yweather:wind/@speed").value[0].content;

print(title);
print("Temperature: " + temperature + " " + temp_unit +
           " (wind chill: " + wind_chill + " " + temp_unit + ")");
print("Wind: " + wind_speed + " " + speed_unit +
           " @ " + wind_direction + " degrees");
