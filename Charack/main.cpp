#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "charack/charack.h"
#include "height.h" // terrain functions

#define OBSERVER_HEIGHT		5
#define MOV_SPEED			5
#define GLOBAL_VIEW_SAMPE	15000
#define GLOBAL_VIEW_HEIGHT	40

// Define how much each of the functions above interferes in the terrain generation.
float gWeightsX[CK_MATHC_MAX_FUNCTION] = {1, 1, 1, 1};
float gWeightsZ[CK_MATHC_MAX_FUNCTION] = {1, 1, 1, 1};

// Some variables to control our settings
int gCurrentSample	= 2;
int gCurrentHeight	= 0;
int gSeaLevel		= CK_SEA_LEVEL;

// We create an "eye" to see the generated world.
CharackWorld gWorld(300, 2);


// To avoid walk through the walls, below the ground, etc.
void sanitizePosition() {
	float	aCameraHeight	= gWorld.getObserver()->getPosition()->y,
			aTerrainHeight	= gWorld.getHeightAtObserverPosition();

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
			gWorld.getObserver()->moveForward(MOV_SPEED * gWorld.getSample());
			break;

		case 's':
			// Move backward
			gWorld.getObserver()->moveBackward(MOV_SPEED * gWorld.getSample());
			break;

		case 'a':
			// Move left
			gWorld.getObserver()->moveLeft(MOV_SPEED * gWorld.getSample());
			break;

		case 'd':
			// Move right
			gWorld.getObserver()->moveRight(MOV_SPEED * gWorld.getSample());
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
			gWorld.getObserver()->moveUpDown(MOV_SPEED);
			break;

		case 'g':
			// Move down
			gWorld.getObserver()->moveUpDown(-MOV_SPEED);
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

		case 'k':
			// Decrease the scale
			gWorld.setScale(gWorld.getScale() - 0.2);
			break;
		case 'l':
			// Increase the scale
			gWorld.setScale(gWorld.getScale() + 0.2);
			break;

		case 'u':
			// Decrease the sea level
			gSeaLevel -= 1;
			break;
		case 'i':
			// Increase the sea level
			gSeaLevel += 1;
			break;

		case 'p':
			// Toggle controller for global viewing (view from top).
			if(gWorld.getObserver()->getRotationX() == 90) {
				gWorld.setSample(gCurrentSample);
				
				gWorld.getObserver()->setRotationX(0);
				gWorld.getObserver()->setPosition(gWorld.getObserver()->getPosition()->x, gCurrentHeight, gWorld.getObserver()->getPosition()->z);
			} else {
				gWorld.getObserver()->setRotationX(90);
				gWorld.getObserver()->setRotationY(0);
				gWorld.getObserver()->setPosition(gWorld.getObserver()->getPosition()->x, GLOBAL_VIEW_HEIGHT, gWorld.getObserver()->getPosition()->z);
				
				gCurrentSample = gWorld.getSample();
				gCurrentHeight = gWorld.getObserver()->getPosition()->y;
				gWorld.setSample(GLOBAL_VIEW_SAMPE);
			}
			break;
	}
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
	glEnable(GL_COLOR_MATERIAL); // enable color tracking
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);// set material properties which will be assigned by glColor


	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); //GL_LINE, GL_FILL, GL_POINT 
}


void displayOcean(void) {
	glBegin(GL_POLYGON);
		glColor3f(0.0f, 0.0f, 0.2f);

		glNormal3f(0,1,0);
		
		glVertex3f(0.0, gSeaLevel, 0.0);
		glVertex3f(CK_VIEW_FRUSTUM, gSeaLevel, 0.0);
		glVertex3f(CK_VIEW_FRUSTUM, gSeaLevel, CK_VIEW_FRUSTUM);
		glVertex3f(0.0, gSeaLevel, CK_VIEW_FRUSTUM);
	glEnd(); 
}

void display (void) {
	setupEnableStuffs();
	setupLights();

	glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 

	gluLookAt(0.0, 0.0, gWorld.getViewFrustum()/2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	//sanitizePosition();
	gWorld.displayMap();
	displayOcean();
	
	system("cls");
	gWorld.printDebugInfo();

	glutSwapBuffers();
}

void init (void) {
	gWorld.getMathCollectionX()->addFunction(fx1);
	gWorld.getMathCollectionX()->addFunction(fx2);
	//gWorld.getMathCollectionX()->addFunction(fx3);
	gWorld.getMathCollectionX()->setWeights(gWeightsX);

	gWorld.getMathCollectionZ()->addFunction(fz1);
	gWorld.getMathCollectionZ()->addFunction(fz2);
	//gWorld.getMathCollectionZ()->addFunction(fz3);
	gWorld.getMathCollectionZ()->setWeights(gWeightsZ);

	//gWorld.placeObserverOnLand();
}

void reshape (int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 10.0, 9000000.0);
	glMatrixMode(GL_MODELVIEW);
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