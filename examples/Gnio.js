#!/usr/bin/env seed
Gio = imports.gi.Gio;
Gnio = imports.gi.Gnio;

r = new Gnio.Resolver();

address = r.lookup_name("www.google.com");
saddr = new Gnio.InetSocketAddress({address: address, port: 80});
client = new Gnio.SocketConnection({address: saddr});

client.connect(null);

message = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";

os = client.get_output_stream();
ds = new Gio.DataOutputStream.c_new(os);
ds.put_string(message);

os.flush();

input = new Gio.DataInputStream.c_new(client.get_input_stream());

out = input.read_until("", 0);
print(out);
