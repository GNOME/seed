girdir = $(datadir)/gir
gir_DATA = GL-1.0.gir GLUT-1.0.gir GLTypes-1.0.gir

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = GL-1.0.typelib GLUT-1.0.typelib GLTypes-1.0.typelib


GL-1.0.typelib: GLTypes-1.0.typelib
	g-ir-compiler --includedir=. GL-1.0.gir -o GL-1.0.typelib
GLTypes-1.0.typelib:
	g-ir-compiler --includedir=. GLTypes-1.0.gir -o GLTypes-1.0.typelib
GLUT-1.0.typelib: GL-1.0.typelib
	g-ir-compiler --includedir=. GLUT-1.0.gir -o GLUT-1.0.typelib
	
