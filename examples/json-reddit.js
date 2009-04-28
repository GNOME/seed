#!/usr/bin/env seed

Gio = imports.gi.Gio;

var redditurl = Gio.file_new_for_uri("http://reddit.com/.json");
var reddit = JSON.parse(redditurl.read().get_contents());

stories = reddit.data.children;

for (i in stories){
    var story = stories[i].data;
    Seed.print("======");
    Seed.print(story.ups + "/" +
	       story.downs + " " +
	       story.title + " (" +
	       story.domain +")" );
    Seed.print(story.author);
}

