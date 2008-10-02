#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <list>

#include "../Charack/charack/vector3.h"

#define _CRT_SECURE_NO_WARNINGS
#define CAMERA_DISTANCE				100.0
#define MAX_DIVISION				3
#define MAX_VARIATION				30

#define rrand(a, b) (int)((a) + rand()/(RAND_MAX + 1.0) * ((b) - (a) + 1))

using namespace std;

bool comparePoints(Vector3 theFirst, Vector3 theSecond) {
	return theFirst.x < theSecond.x;
}


list<Vector3> midpointFractal(int theMaxDivision, Vector3 thePointA, Vector3 thePointB) {
	list<Vector3> aResult;	
	list<Vector3> aSegments;

	srand(thePointA.x);
	float aRandRange = MAX_VARIATION;

	aSegments.push_back(thePointA);
	aSegments.push_back(thePointB);
	aSegments.push_back(Vector3(aRandRange, theMaxDivision, 0));

	aResult.push_back(thePointA);
	
	while(aSegments.size() > 0) {
		Vector3 aPointA = aSegments.front();
		aSegments.pop_front();

		Vector3 aPointB = aSegments.front();
		aSegments.pop_front();

		Vector3 aSettings = aSegments.front();
		aSegments.pop_front();

		Vector3 aMidPoint = (aPointA + aPointB)/2.0;
		aMidPoint.y = aMidPoint.y + rrand(-aSettings.x, aSettings.x); // TODO: support to z and x rand variation...

		aSettings.x = aSettings.x/2;
		aSettings.y = aSettings.y - 1;

		if(aSettings.y > 0) {
			aSegments.push_back(aPointA);
			aSegments.push_back(aMidPoint);
			aSegments.push_back(aSettings);

			aSegments.push_back(aMidPoint);
			aSegments.push_back(aPointB);
			aSegments.push_back(aSettings);
		}

		aResult.push_back(aMidPoint);		
	}

	aResult.push_back(thePointB);
	aResult.sort(comparePoints);

	return aResult;
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
	}
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 0.0);
	glDisable(GL_POINT_SMOOTH);
	glPointSize(2.0);

	list<Vector3> aResult;	
	list<Vector3>::iterator i;

	aResult = midpointFractal(MAX_DIVISION, Vector3(-50,0,0), Vector3(50, 0, 0));

	glBegin(GL_LINE_STRIP); //GL_POINTS
		for(i = aResult.begin(); i != aResult.end(); i++) {
			Vector3 aPoint = (*i);
			//printf("(%.2f,%.2f,%.2f)\n", aPoint.x, aPoint.y, aPoint.z);
			glVertex2f(aPoint.x, aPoint.y);
		}
	glEnd();

	glutSwapBuffers();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();
	glOrtho(-CAMERA_DISTANCE, CAMERA_DISTANCE, -CAMERA_DISTANCE, CAMERA_DISTANCE, -CAMERA_DISTANCE, CAMERA_DISTANCE);
}

int main (int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Continent generator");
	
	init();
	
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(processNormalKeys);
	
	glutMainLoop();
	
	return 0;
}