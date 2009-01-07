#!/usr/bin/env seed

Seed.import_namespace("Soup");

var session = new Soup.SessionSync();

var uri = new Soup.URI.c_new("http://www.google.com");
var request = new Soup.Message({method:"GET", uri:uri});

var status = session.send_message(request);

Seed.print(status);

