AM_CPPFLAGS = \
	$(SEED_CFLAGS)

AM_CFLAGS =\
	 -Wall -I../../libseed

bin_PROGRAMS = seed_turtle

seed_turtle_SOURCES = \
	turtle.c

seed_turtle_LDFLAGS = \
	-L../../libseed/.libs -lseed -lreadline

seed_turtle_LDADD = \
	$(SEED_LIBS) \
	-lseed
