#!/usr/bin/env seed
GL = imports.gi.GL;
GLUT = imports.gi.GLUT;


var mat_specular = [1.0,1.0,1.0,1.0];
var mat_shininess = [50.0];
var light_position = [0.0,0.0,-1.0,0.0];

GLUT.Init(null, null);

GLUT.InitDisplayMode(GLUT.DEPTH | GLUT.RGBA | GLUT.DOUBLE);


GLUT.InitWindowSize(640, 480);

GLUT.CreateWindow("Oh dear god");


GLUT.DisplayFunc(
    function(){
	with (GL){
	    Clear(DEPTH_BUFFER_BIT | COLOR_BUFFER_BIT);
	    PushMatrix();
	    Begin(TRIANGLES);
	    Color3f(1,0,0);
	    Vertex3f(-1,-1,0);
	    Color3f(0,1,0);
	    Vertex3f(1,-1,0);
	    Color3f(0,0,1);
	    Vertex3f(0,1,0);
	    End();		
	    PopMatrix();
	}
	GLUT.SolidTeapot(0.5);
	GLUT.SwapBuffers();
    });


GL.ClearColor(0.0,0.0,0.0,0.0);
GL.ShadeModel(GL.SMOOTH);

//GL.glMaterialfv(GL.FRONT, GL.SPECULAR, mat_specular);
//GL.glMaterialfv(GL.FRONT, GL.SHININESS, mat_shininess);
GL.Lightfv(GL.LIGHT0, GL.POSITION, light_position);

GL.Enable(GL.LIGHTING);
GL.Enable(GL.LIGHT0);


GLUT.MainLoop();

