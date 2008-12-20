#!/usr/bin/env seed
Seed.import_namespace("Gio");
Seed.import_namespace("Gnio");

r = new Gnio.Resolver();

address = r.lookup_name("www.google.com");
saddr = new Gnio.InetSocketAddress({address: address, port: 80});
client = new Gnio.SocketConnection({address: saddr});

client.connect(null);

message = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";

os = client.get_output_stream();
ds = Gio.DataOutputStream._new(os);
ds.put_string(message);

os.flush();

input = Gio.DataInputStream._new(client.get_input_stream());

out = input.read_until("", 0);
Seed.print(out);