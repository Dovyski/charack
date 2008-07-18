#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "charack/charack.h"

// All this funciton will be interpolated to create the world terrain.
// The weight of each one of these functions are described in gWeights (below).
float f1(float a) { return sin(a); }
float f2(float a) { return sin(20*a); }
float f3(float a) { return sin(300*a); }
float f4(float a) { return sin(500*a); }

// Define how much each of the functions above interferes in the terrain generation.
float gWeights[CK_MATHC_MAX_FUNCTION] = {100, 30, 3, 1};


// We create an "eye" to see the generated world.
CharackRender gRender;

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'w':
			// Move forward
			gRender.getCamera()->moveForward(5);
			break;

		case 's':
			// Move backward
			gRender.getCamera()->moveBackward(5);
			break;

		case 'a':
			// Move left
			gRender.getCamera()->moveLeft(5);
			break;

		case 'd':
			// Move right
			gRender.getCamera()->moveRight(5);
			break;

		case 'r':
			// Look above
			gRender.getCamera()->rotateLookUpDown(5);
			break;

		case 'f':
			// Look down
			gRender.getCamera()->rotateLookUpDown(-5);
			break;

		case 'q':
			// Look to the left
			gRender.getCamera()->rotateLookLeftRight(-5);
			break;

		case 'e':
			// Look to the right
			gRender.getCamera()->rotateLookLeftRight(5);
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
	
	gRender.displayMap();

	glutSwapBuffers();
}

void init (void) {
	gRender.getMathCollection()->addFunction(f1);
	gRender.getMathCollection()->addFunction(f2);
	gRender.getMathCollection()->addFunction(f3);
	gRender.getMathCollection()->addFunction(f4);

	gRender.getMathCollection()->setWeights(gWeights);
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