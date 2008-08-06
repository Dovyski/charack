#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "charack/charack.h"

#define OBSERVER_HEIGHT		30

// All this funciton will be interpolated to create the world terrain.
// The weight of each one of these functions are described in gWeights (below).
float f1(float a) { return sin(a); }
float f2(float a) { return sin(20*a); }
float f3(float a) { return sin(300*a); }
float f4(float a) { return sin(500*a); }

// Define how much each of the functions above interferes in the terrain generation.
float gWeights[CK_MATHC_MAX_FUNCTION] = {2000, 500, 50, 1};


// We create an "eye" to see the generated world.
CharackRender gRender;


// To avoid walk through the walls, below the ground, etc.
void sanitizePosition() {
	float	aCameraHeight	= gRender.getCamera()->getPosition()->y,
			aTerrainHeight	= gRender.getHeight(gRender.getCamera()->getPosition()->x, gRender.getCamera()->getPosition()->z);

	if((aCameraHeight - OBSERVER_HEIGHT) < aTerrainHeight) {
		gRender.getCamera()->getPosition()->y = aTerrainHeight + OBSERVER_HEIGHT;
	}
}

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

		case 't':
			// Move up
			gRender.getCamera()->moveUpDown(5);
			break;

		case 'g':
			// Move down
			gRender.getCamera()->moveUpDown(-5);
			break;
	}


	sanitizePosition();
}

void setupLights(void) {
	// Create light components
	GLfloat aAmbientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat aDiffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat aSpecularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat aPosition[] = { -1.5f, 1.0f, -4.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, aAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, aDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, aSpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, aPosition);
}


void setupEnableStuffs(void) {
	glEnable(GL_DEPTH_TEST);	//enable the depth testing
	glDepthFunc(GL_LEQUAL);		//set the depth function
	glFrontFace(GL_CW);		//set which face is facing forward
//	glCullFace(GL_BACK);		//set the face to be culled
//	glEnable(GL_CULL_FACE);		//enable culling to speed up the processing time
	glShadeModel(GL_SMOOTH);	//set the shade model to smooth
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);		//enable our ambient light
	glEnable(GL_NORMALIZE);		//enable normalizing of normals, wont make a difference if disabled

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); //GL_LINE, GL_FILL, GL_POINT 
}


void display (void) {
	glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 

	gluLookAt(0.0, 0.0, CK_VIEW_FRUSTUM/2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	gRender.displayMap();

	glutSwapBuffers();
}

void init (void) {
	setupEnableStuffs();
	setupLights();

	gRender.getMathCollectionX()->addFunction(f1);
//	gRender.getMathCollectionX()->addFunction(f2);
//	gRender.getMathCollectionX()->addFunction(f3);
//	gRender.getMathCollectionX()->addFunction(f4);

	gRender.getMathCollectionX()->setWeights(gWeights);

	gRender.getMathCollectionZ()->addFunction(f1);
//	gRender.getMathCollectionZ()->addFunction(f2);
//	gRender.getMathCollectionZ()->addFunction(f3);
//	gRender.getMathCollectionZ()->addFunction(f4);

	gRender.getMathCollectionZ()->setWeights(gWeights);
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