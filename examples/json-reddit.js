#!/usr/bin/env seed

Gio = imports.gi.Gio;

JSON = imports.JSON;

var redditurl = Gio.file_new_for_uri("http://reddit.com/.json");
var reddit = JSON.parse(redditurl.read().get_contents());

stories = reddit.data.children;

for (i in stories){
    var story = stories[i].data;
    print("======");
    print(story.ups + "/" +
	       story.downs + " " +
	       story.title + " (" +
	       story.domain +")" );
    print(story.author);
}

