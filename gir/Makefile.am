CAIRO_INCLUDEDIR=`pkg-config --variable=includedir cairo`

Cairo-1.0.gir: 
	g-ir-scanner -v --namespace Cairo --nsversion=1.0 \
	--strip-prefix=Cairo --add-include-path=/usr/local/share/gir \
	--include=fontconfig-2.0 \
	--include=freetype2-2.0 \
	--library=cairo \
	--output Cairo-1.0.gir \
	--pkg freetype2 \
	-I`pkg-config --variable=includedir cairo` \
	$(CAIRO_INCLUDEDIR)/cairo/cairo.h \
	$(CAIRO_INCLUDEDIR)/cairo/cairo-ft.h \
	$(CAIRO_INCLUDEDIR)/cairo/cairo-pdf.h \
	$(CAIRO_INCLUDEDIR)/cairo/cairo-ps.h \
	$(CAIRO_INCLUDEDIR)/cairo/cairo-svg.h

Cairo-1.0.typelib: Cairo-1.0.gir
	g-ir-compiler --includedir=$(datadir)/dir Cairo-1.0.gir -o Cairo-1.0.typelib

girdir = $(datadir)/gir
gir_DATA = Cairo-1.0.gir

typelibsdir = $(libdir)/girepository
typelibs_DATA = Cairo-1.0.typelib


	
