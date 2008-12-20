#!/usr/local/bin/seed
Seed.import_namespace("Gtk");
Seed.import_namespace("Gst");
Seed.import_namespace("GstVideo");

Gtk.init(null, null);
Gst.init(null, null);

pipeline = new Gst.Pipeline({name: "VideoTest"});

source = Gst.ElementFactory.make("videotestsrc", "video");
sink = Gst.ElementFactory.make("autovideosink", "sink");


pipeline.add(source);
pipeline.add(sink);
source.link(sink);

pipeline.set_state(Gst.State.Playing);

Gtk.main();



