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
float fs1(float a) { return sin(a/(CK_MAX_WIDTH/20)); }
float fs2(float a) { return sin(a/(CK_MAX_WIDTH/20000)); }
float fs3(float a) { return cos(a/(CK_MAX_WIDTH/200000)); }

float fc1(float a) { return cos(a/(CK_MAX_WIDTH/20)); }
float fc2(float a) { return cos(a/(CK_MAX_WIDTH/20000)); }
float fc3(float a) { return cos(a/(CK_MAX_WIDTH/200000)); }


// Define how much each of the functions above interferes in the terrain generation.
float gWeightsX[CK_MATHC_MAX_FUNCTION] = {80, 60, 2, 1};
float gWeightsZ[CK_MATHC_MAX_FUNCTION] = {50, 120, 5, 1};


// We create an "eye" to see the generated world.
CharackWorld gWorld(300, 1);


// To avoid walk through the walls, below the ground, etc.
void sanitizePosition() {
	float	aCameraHeight	= gWorld.getObserver()->getPosition()->y,
			aTerrainHeight	= gWorld.getHeight(gWorld.getObserver()->getPosition()->x, gWorld.getObserver()->getPosition()->z);

	if((aCameraHeight - OBSERVER_HEIGHT) < aTerrainHeight) {
		gWorld.getObserver()->getPosition()->y = aTerrainHeight + OBSERVER_HEIGHT;
	}
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'w':
			// Move forward
			gWorld.getObserver()->moveForward(5);
			break;

		case 's':
			// Move backward
			gWorld.getObserver()->moveBackward(5);
			break;

		case 'a':
			// Move left
			gWorld.getObserver()->moveLeft(5);
			break;

		case 'd':
			// Move right
			gWorld.getObserver()->moveRight(5);
			break;

		case 'r':
			// Look above
			gWorld.getObserver()->rotateLookUpDown(5);
			break;

		case 'f':
			// Look down
			gWorld.getObserver()->rotateLookUpDown(-5);
			break;

		case 'q':
			// Look to the left
			gWorld.getObserver()->rotateLookLeftRight(-5);
			break;

		case 'e':
			// Look to the right
			gWorld.getObserver()->rotateLookLeftRight(5);
			break;

		case 't':
			// Move up
			gWorld.getObserver()->moveUpDown(5);
			break;

		case 'g':
			// Move down
			gWorld.getObserver()->moveUpDown(-5);
			break;

		case 'c':
			// Decrease the view frustum (user can see short distances)
			gWorld.setViewFrustum(gWorld.getViewFrustum() - 5);
			break;
		case 'v':
			// Increase the view frustum (user can see large distances)
			gWorld.setViewFrustum(gWorld.getViewFrustum() + 5);
			break;

		case 'n':
			// Decrease the sample size
			gWorld.setSample(gWorld.getSample() - 1);
			break;
		case 'm':
			// Increase the sample size
			gWorld.setSample(gWorld.getSample() + 1);
			break;
	}


	sanitizePosition();
}

void setupLights(void) {
	// Create light components
	GLfloat aAmbientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat aDiffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat aSpecularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat aPosition[] = { 0.0f, 500.0f, 0.0f, 1.0f };

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
	setupEnableStuffs();
	setupLights();

	glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 

	gluLookAt(0.0, 0.0, gWorld.getViewFrustum()/2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	sanitizePosition();
	gWorld.displayMap();
	
	system("cls");
	gWorld.printDebugInfo();

	glutSwapBuffers();
}

void init (void) {
	gWorld.getMathCollectionX()->addFunction(fs1);
	gWorld.getMathCollectionX()->addFunction(fs2);
	gWorld.getMathCollectionX()->addFunction(fs3);
	gWorld.getMathCollectionX()->setWeights(gWeightsX);

	gWorld.getMathCollectionZ()->addFunction(fc1);
	gWorld.getMathCollectionZ()->addFunction(fc2);
	gWorld.getMathCollectionZ()->addFunction(fc3);
	gWorld.getMathCollectionZ()->setWeights(gWeightsZ);
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