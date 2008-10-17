all:
	gcc *.c -shared -fPIC -lreadline -g -o libseed.so `pkg-config --cflags --libs webkit-1.0 gobject-introspection-1.0 glib-2.0`
	cp libseed.so ..
install:
	cp libseed.so /usr/local/lib
