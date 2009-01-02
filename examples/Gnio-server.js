#!/usr/bin/env seed
Seed.import_namespace("Gnio");
Seed.import_namespace("Gio");

// I don't think this is the right way of doing things.
var r = new Gnio.Resolver();
var sock = new Gnio.Socket({domain: Gnio.SocketDomain.Inet,
						    type: Gnio.SocketType.Stream});


var addr = r.lookup_name("localhost");
						
sock.bind(new Gnio.InetSocketAddress({address: addr,
 				                      port: 9999}));

Seed.print("Echo server listening on port 9999");
sock.listen(true);

var client = sock.accept();

Seed.print("Accepted client");

ds = Gio.DataInputStream._new(new Gnio.SocketInputStream({socket: client}));
os = Gio.DataOutputStream._new(new Gnio.SocketOutputStream({socket: client}));
os.put_string("Seed echo server. Type quit to quit.\n");

while(1)
{
	var line = ds.read_line(null);
        var cowsay = Seed.spawn("cowsay " + line);
	if (line.search("quit") > -1)
	{
		client.close();
		Seed.quit();
	}
	os.put_string(cowsay.stdout);
	os.put_string("\n");
}


client.close();

