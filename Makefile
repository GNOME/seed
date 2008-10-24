all:
	gcc libseed/*.c -Wall -O3  -shared -fPIC -lreadline -g -o libseed/libseed.so `pkg-config --cflags --libs webkit-1.0 gobject-introspection-1.0 glib-2.0`
	gcc `pkg-config --cflags --libs gstreamer-0.10 glib-2.0 webkit-1.0 gobject-introspection-1.0` *.c -g -o seed  -L libseed -lseed -lreadline -lffi
install:
	cp libseed/libseed.so /usr/local/lib
	cp ./seed /usr/local/bin/seed 
test:
	cd tests ; ./run-tests.py
