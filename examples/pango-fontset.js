#!/usr/bin/env seed

Pango = imports.gi.Pango;
PangoFT2 = imports.gi.PangoFT2;

context = new Pango.Context();
description = new Pango.FontDescription.c_new();
description.set_family("");
fontmap = new PangoFT2.FontMap();
fontset = fontmap.load_fontset(context,
			       description,
			       Pango.language_get_default());
fontset.foreach(function(fontset, font){
    description = font.describe();
    print(description.to_string());
});
