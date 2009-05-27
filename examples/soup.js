#!/usr/bin/env seed

Soup = imports.gi.Soup;

var session = new Soup.SessionSync();

// Soup.URI is a struct.
var uri = new Soup.URI.c_new("http://www.google.com");
var request = new Soup.Message({method:"GET", uri:uri});

var status = session.send_message(request);

print(status);

