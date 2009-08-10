#!/usr/bin/env seed

cairo = imports.cairo;
Gtk = imports.gi.Gtk;

const dashes = [50.0, 10.0, 10.0, 10.0];
const offset = -50.0;

function draw_arc(drawing_area){
    var cr = new cairo.Context.from_drawable(drawing_area.window);

    cr.scale(2, 2);

    cr.operator = cairo.Operator.CLEAR;
    cr.paint();
    cr.operator = cairo.Operator.OVER;

    var pat = new cairo.RadialGradient(115.2, 102.4, 25.6, 102.4, 102.4, 128);
    pat.add_color_stop_rgba (0, 1, 1, 1, 1);
    pat.add_color_stop_rgba (1, 0, 0, 0, 1);
    cr.set_source (pat);
    cr.arc(128, 128, 76.8, 0, 2*Math.PI);
    cr.fill();

    return false;
}

Gtk.init(Seed.argv);

var w = new Gtk.Window();
w.signal.hide.connect(Gtk.main_quit);
var d = new Gtk.DrawingArea();
w.add(d);

w.resize(500,600);

w.decorated = false;


d.signal.expose_event.connect(draw_arc);

w.set_colormap(w.screen.get_rgba_colormap());

w.show_all();
Gtk.main();
