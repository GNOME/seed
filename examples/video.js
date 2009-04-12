#!/usr/bin/env seed
Gtk = imports.gi.Gtk;
Gst = imports.gi.Gst;
GstVideo = imports.gi.GstVideo;

Gtk.init(null, null);
Gst.init(null, null);

pipeline = new Gst.Pipeline({name: "VideoTest"});

source = Gst.ElementFactory.make("videotestsrc", "video");
sink = Gst.ElementFactory.make("autovideosink", "sink");

pipeline.add(source);
pipeline.add(sink);
source.link(sink);

pipeline.set_state(Gst.State.PLAYING);

Gtk.main();

