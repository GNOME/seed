#!/usr/bin/env seed
Seed.import_namespace("Soup");

session = new Soup.SessionSync();
uri = Soup.URI._new("http://www.google.com");
request = new Soup.Message({method:"GET",
			    uri:uri});
status = session.send_message(request);
Seed.print(status);
			    

