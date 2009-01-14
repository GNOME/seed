if BUILD_SEED_EXAMPLES
if BUILD_TURTLE_EXAMPLE

AM_CPPFLAGS = \
	$(SEED_CFLAGS) `pkg-config --cflags gtk+-2.0`

AM_CFLAGS =\
	 -Wall -I../../libseed

bin_PROGRAMS = seed_turtle

seed_turtle_SOURCES = \
	turtle.c

seed_turtle_LDFLAGS = \
	-L../../libseed/.libs -lseed `pkg-config --libs gtk+-2.0`

seed_turtle_LDADD = \
	$(SEED_LIBS) \
	-lseed

endif
endif
