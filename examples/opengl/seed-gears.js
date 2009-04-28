#!/usr/bin/env seed
GL = imports.gi.GL;
GLUT = imports.gi.GLUT;

function gear(inner_radius, outer_radius, width, teeth, tooth_depth){
    var i, angle;
    var r0 = inner_radius;
    var r1 = outer_radius - tooth_depth / 2.0;
    var r2 = outer_radius + tooth_depth / 2.0;
    
    var da = 2.0 * Math.PI / teeth/ 4.0;
    
    GL.ShadeModel(GL.FLAT);
    GL.Normal3f(0.0, 0.0, 1.0);
    
    GL.Begin(GL.QUAD_STRIP);
    
    for (i = 0; i <= teeth; i++)
    {
	angle = i * 2.0 * Math.PI / teeth;
	GL.Vertex3f(r0 * Math.cos(angle), r0 * Math.sin(angle), width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle), r1 * Math.sin(angle), width * 0.5);
	GL.Vertex3f(r0 * Math.cos(angle), r0 * Math.sin(angle), width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle + 3 * da), 
		    r1 * Math.sin(angle + 3 * da), width * 0.5);
    }

    GL.Begin(GL.QUADS);
    da = 2.0 * Math.PI / teeth / 4.0;
    for (i = 0; i < teeth; i++)
    {
	angle = (i) * 2.0 * Math.PI / teeth;

	GL.Vertex3f(r2 * Math.cos(angle + 2 * da), 
		    r2 * Math.sin(angle + 2 * da), width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle + 3 * da), 
		    r1 * Math.sin(angle + 3 * da), width * 0.5);

	GL.Vertex3f(r2 * Math.cos(angle + da), 
		    r2 * Math.sin(angle + da), width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle), r1 * Math.sin(angle), width * 0.5);
    }
    GL.End();

    
    GL.Normal3f(0.0, 0.0, -1.0);


    /* draw back face */
    GL.Begin(GL.QUAD_STRIP);
    for (i = 0; i <= teeth; i++)
    {
	angle = i * 2.0 * Math.PI / teeth;
	GL.Vertex3f(r1 * Math.cos(angle), r1 * Math.sin(angle), -width * 0.5);
	GL.Vertex3f(r0 * Math.cos(angle), r0 * Math.sin(angle), -width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle + 3 * da), r1 * Math.sin(angle + 3 * da), -width * 0.5);
	GL.Vertex3f(r0 * Math.cos(angle), r0 * Math.sin(angle), -width * 0.5);
    }
    GL.End();

    /* draw back sides of teeth */
    GL.Begin(GL.QUADS);
    da = 2.0 * Math.PI / teeth / 4.0;
    for (i = 0; i < teeth; i++)
    {
	angle = i * 2.0 * Math.PI / teeth;

	GL.Vertex3f(r1 * Math.cos(angle + 3 * da), r1 * Math.sin(angle + 3 * da), -width * 0.5);
	GL.Vertex3f(r2 * Math.cos(angle + 2 * da), r2 * Math.sin(angle + 2 * da), -width * 0.5);
	GL.Vertex3f(r2 * Math.cos(angle + da), r2 * Math.sin(angle + da), -width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle), r1 * Math.sin(angle), -width * 0.5);
    }
    GL.End();

    /* draw outward faces of teeth */
    GL.Begin(GL.QUAD_STRIP);
    for (i = 0; i < teeth; i++)
    {
	angle = i * 2.0 * Math.PI / teeth;

	GL.Vertex3f(r1 * Math.cos(angle), r1 * Math.sin(angle), width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle), r1 * Math.sin(angle), -width * 0.5);
	u = r2 * Math.cos(angle + da) - r1 * Math.cos(angle);
	v = r2 * Math.sin(angle + da) - r1 * Math.sin(angle);
	len = Math.sqrt(u * u + v * v);
	u /= len;
	v /= len;
	GL.Normal3f(v, -u, 0.0);
	GL.Vertex3f(r2 * Math.cos(angle + da), r2 * Math.sin(angle + da), width * 0.5);
	GL.Vertex3f(r2 * Math.cos(angle + da), r2 * Math.sin(angle + da), -width * 0.5);
	GL.Normal3f(Math.cos(angle), Math.sin(angle), 0.0);
	GL.Vertex3f(r2 * Math.cos(angle + 2 * da), r2 * Math.sin(angle + 2 * da), width * 0.5);
	GL.Vertex3f(r2 * Math.cos(angle + 2 * da), r2 * Math.sin(angle + 2 * da), -width * 0.5);
	u = r1 * Math.cos(angle + 3 * da) - r2 * Math.cos(angle + 2 * da);
	v = r1 * Math.sin(angle + 3 * da) - r2 * Math.sin(angle + 2 * da);
	GL.Normal3f(v, -u, 0.0);
	GL.Vertex3f(r1 * Math.cos(angle + 3 * da), r1 * Math.sin(angle + 3 * da), width * 0.5);
	GL.Vertex3f(r1 * Math.cos(angle + 3 * da), r1 * Math.sin(angle + 3 * da), -width * 0.5);
	GL.Normal3f(Math.cos(angle), Math.sin(angle), 0.0);
    }

    GL.Vertex3f(r1 * Math.cos(0), r1 * Math.sin(0), width * 0.5);
    GL.Vertex3f(r1 * Math.cos(0), r1 * Math.sin(0), -width * 0.5);

    GL.End();

    GL.ShadeModel(GL.SMOOTH);

    /* draw inside radius cylinder */
    GL.Begin(GL.QUAD_STRIP);
    for (i = 0; i <= teeth; i++)
    {
	angle = i * 2.0 * Math.PI / teeth;
	GL.Normal3f(-Math.cos(angle), -Math.sin(angle), 0.0);
	GL.Vertex3f(r0 * Math.cos(angle), r0 * Math.sin(angle), -width * 0.5);
	GL.Vertex3f(r0 * Math.cos(angle), r0 * Math.sin(angle), width * 0.5);
    }
    GL.End();

}


var view_rotx = 20.0;
var view_roty = 30.0;
var view_rotz = 0.0;
var langle = 0.0;
var count = 1;


function draw(){
    with(GL){
	Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);
	
	PushMatrix();
	Rotatef(view_rotx, 1.0, 0.0, 0.0);
	Rotatef(view_roty, 0.0, 1.0, 0.0);
	Rotatef(view_rotz, 0.0, 0.0, 1.0);
	
	PushMatrix();
	Translatef(-3.0, -2.0, 0.0);
	Rotatef(langle, 0.0, 0.0, 1.0);
	CallList(gear1);
	PopMatrix();
	
	PushMatrix();
	Translatef(3.1, -2.0, 0.0);
	Rotatef(-2.0 * langle - 9.0, 0.0, 0.0, 1.0);
	CallList(gear2);
	PopMatrix();
	
	PushMatrix();
	Translatef(-3.1, 4.2, 0.0);
	Rotatef(-2.0 * langle - 25.0, 0.0, 0.0, 1.0);
	CallList(gear3);
	PopMatrix();
	
	
	PopMatrix();
	
	GLUT.SwapBuffers();
    }
}

function idle(){
    langle += 0.1;
    GLUT.PostRedisplay();
}

function reshape(width, height){
    var h = height/width;
    
    with(GL){
	Viewport(0, 0, width, height);
	MatrixMode(PROJECTION);
	LoadIdentity();
	Frustum(-1.0, 1.0, -h, h, 5.0, 60.0);
	MatrixMode(MODELVIEW);
	LoadIdentity();
	Translatef(0.0, 0.0, -40.0);
    }
}

function init(){
    pos = [5.0, 5.0, 10.0, 0.0];
    red = [0.8, 0.1, 0.0, 1.0];
    green = [0.0, 0.8, 0.2, 1.0];
    blue = [0.2, 0.2, 1.0, 1.0];
    
    with (GL){
	Lightfv(LIGHT0, POSITION, pos);
	Enable(CULL_FACE);
	Enable(LIGHTING);
	Enable(LIGHT0);
	Enable(DEPTH_TEST);
	
	gear1 = GenLists(1);
	NewList(gear1, COMPILE);
	Materialfv(FRONT, AMBIENT_AND_DIFFUSE, red);
	gear(1.0, 4.0, 1.0, 20, 0.7);
	EndList();
	
	gear2 = GenLists(1);
	NewList(gear2, COMPILE);
	Materialfv(FRONT, AMBIENT_AND_DIFFUSE, green);
	gear(0.5, 2.0, 2.0, 10, 0.7);
	EndList();
	
	gear3 = GenLists(1);
	NewList(gear3, COMPILE);
	Materialfv(FRONT, AMBIENT_AND_DIFFUSE, blue);
	gear(1.3, 2.0, 0.5, 10, 0.7);
	EndList();
	
	Enable(NORMALIZE);		
    }
}

GLUT.Init(null, null);

GLUT.InitDisplayMode(GLUT.RGB | GLUT.DEPTH | GLUT.DOUBLE);
GLUT.CreateWindow("Gears");

init();

GLUT.DisplayFunc(draw);
GLUT.IdleFunc(idle);
GLUT.ReshapeFunc(reshape);

GLUT.MainLoop();
