#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "charack/charack.h"

CharackCamera gCamera;

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'w':
			// Move forward
			gCamera.moveForward(5);
			break;

		case 's':
			// Move backward
			gCamera.moveBackward(5);
			break;

		case 'a':
			// Move left
			gCamera.moveLeft(5);
			break;

		case 'd':
			// Move right
			gCamera.moveRight(5);
			break;

		case 'r':
			// Look above
			gCamera.rotateLookUpDown(5);
			break;

		case 'f':
			// Look down
			gCamera.rotateLookUpDown(-5);
			break;

		case 'q':
			// Look to the left
			gCamera.rotateLookLeftRight(5)
			break;

		case 'e':
			// Look to the right
			gCamera.rotateLookLeftRight(-5);
			break;
	}
}



void setupEnableStuffs(void) {
	glEnable(GL_DEPTH_TEST);	//enable the depth testing
	glDepthFunc(GL_LEQUAL);		//set the depth function
	glFrontFace(GL_CCW);		//set which face is facing forward
	glCullFace(GL_BACK);		//set the face to be culled
	glEnable(GL_CULL_FACE);		//enable culling to speed up the processing time
	glShadeModel (GL_SMOOTH);	//set the shade model to smooth
	glEnable (GL_LIGHT1);		//enable our ambient light
	glEnable(GL_NORMALIZE);		//enable normalizing of normals, wont make a difference if disabled
}


void display (void) {
	glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 
	
	setupEnableStuffs();

	gluLookAt(0.0, 0.0, CK_VIEW_FRUSTUM, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// We point the camera to our landscape
	glRotatef(gCamera.getRotationY(), 0,1,0);
	glRotatef(gCamera.getRotationX(), 1,0,0);
	glTranslatef(gCamera.getPosition()->x, gCamera.getPosition()->y, gCamera.getPosition()->z);
	
	displayHeightMapDovyski();
	glutSwapBuffers();
}

void init (void) {
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
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(800, 600); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Pseudo-infinie world generator");

	init();

    glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(processNormalKeys);

	glutMainLoop();

	return 0;
}