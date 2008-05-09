#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "heightmap.h"
#include "move.h"
#include "config.h"

void setupEnableStuffs(void) {
	glEnable(GL_DEPTH_TEST);	//enable the depth testing
	glDepthFunc(GL_LEQUAL);		//set the depth function
	glFrontFace(GL_CCW);		//set which face is facing forward
	glCullFace(GL_BACK);		//set the face to be culled
	glEnable(GL_CULL_FACE);		//enable culling to speed up the processing time
	glShadeModel (GL_SMOOTH);	//set the shade model to smooth
	glEnable(GL_TEXTURE_2D);	//enable texturing
	glEnable(GL_LIGHTING);		//enable the lighting
	glEnable(GL_LIGHT0);		//enable our diffuse light
	glEnable (GL_LIGHT1);		//enable our ambient light
	glEnable(GL_NORMALIZE);		//enable normalizing of normals, wont make a difference if disabled
}

void setupLights(void) {
	GLfloat aDiffuseLight[] = {dlr, dlg, dlb};
	GLfloat aAmbientLight[] = {alr, alg, alb};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, aDiffuseLight);
	glLightfv(GL_LIGHT1, GL_AMBIENT, aAmbientLight);
	
	GLfloat aLightPosition[] = {getPositionX(), getPositionY(), getPositionZ(), lw};
	glLightfv (GL_LIGHT0, GL_POSITION, aLightPosition);
}

void display (void) {
	glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 
	
	setupEnableStuffs();
	setupLights();

	// TODO: colocar uma constante aqui
	gluLookAt(0.0, 0.0, CK_VIEW_FRUSTUM, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// We point the camera to our landscape

	glRotatef(gRotY, 0,1,0);
	glRotatef(gRotX, 1,0,0);
	glTranslatef(gCameraPosition.x, gCameraPosition.y, gCameraPosition.z);
	
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	displayHeightMapDovyski();
	glutSwapBuffers();
}

void init (void) {
	texture[0] = LoadTextureRAW("texture.raw", 256, 256);
	LoadHeightMap ("height.raw", 128, 128);
}

void reshape (int w, int h) {
	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.01, 500.0);
	glMatrixMode (GL_MODELVIEW);
}

int main (int argc, char **argv) {
    glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Pseudo-infinie world generator");

	init();

    glutDisplayFunc (display);
	glutIdleFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc(processNormalKeys);

	glutMainLoop ();

	return 0;
}