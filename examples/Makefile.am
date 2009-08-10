SUBDIRS = \
    browser \
    clutter-pad \
    clutter-shader \
    dbus \
    glib \
    opengl \
    turtle \
    xml \
    twitter

seed_examples = \
    accelgroup.js \
    actions.js \
    async-quine.js \
    calculator.js \
    calculator-mpfr.js \
    cairo.js \
    clutter-1.0.js \
    clutter-cairo-1.0.js \
    clutter-cogl-1.0.js \
    gconf.js \
    gdkpixbufloader.js \
    Gnio.js \
    Gnio-server.js \
    gtkplug.js \
    gtktextview.js \
    gtktreeview.js \
    json-packing.js \
    json-reddit.js \
    ls.js \
    n-oscillator.js \
    notify-test.js \
    pango-fontset.js \
    pango.js \
    poppler.js \
    quine.js \
    repl.js \
    soup.js \
    threaded-repl.js \
    video.js \
    vte-test.js

repldir = $(datadir)/seed
repl_SCRIPTS = repl.js

if BUILD_SEED_EXAMPLES
examplesdir=$(datadir)/doc/seed/examples
examples_DATA = $(seed_examples) HACKING
endif

EXTRA_DIST = $(seed_examples) HACKING
