#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <list>

#include "../Charack/charack/CharackCoastGenerator.h"

#define _CRT_SECURE_NO_WARNINGS
#define CAMERA_DISTANCE				100.0

CharackCoastGenerator gCoaster;

int gMaxDivision	= 8;
int gVariation		= -20;

using namespace std;

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;

		case 'a':
			gMaxDivision--;
			break;

		case 's':
			gMaxDivision++;
			break;

		case 'd':
			gVariation--;
			break;

		case 'f':
			gVariation++;
			break;
	}

	system("cls");
	printf("gMaxDivision = %d\n", gMaxDivision);
	printf("gVariation = %d\n", gVariation);
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 0.0);
	glDisable(GL_POINT_SMOOTH);
	glPointSize(2.0);

	gCoaster.setVariation(gVariation);
	gCoaster.setMaxDivisions(gMaxDivision);

	list<Vector3> aResult;
	list<Vector3>::iterator i;

	gCoaster.setRandSeed(-50);
	aResult = gCoaster.generate(Vector3(-50,0,0), Vector3(50, 0, 0), CharackCoastGenerator::AXIS_Y);

	glBegin(GL_LINE_STRIP); //GL_POINTS
		for(i = aResult.begin(); i != aResult.end(); i++) {
			Vector3 aPoint = (*i);
			glVertex2f(aPoint.x, aPoint.y);
		}
	glEnd();

	gCoaster.setRandSeed(50);	
	aResult = gCoaster.generate(Vector3(50,0,0), Vector3(50, 60, 0), CharackCoastGenerator::AXIS_X);

	glBegin(GL_LINE_STRIP); //GL_POINTS
		for(i = aResult.begin(); i != aResult.end(); i++) {
			Vector3 aPoint = (*i);
			glVertex2f(aPoint.x, aPoint.y);
		}
	glEnd();

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP); //GL_POINTS
		glVertex2f(-50, 0);
		glVertex2f(50, 0);
		glVertex2f(50, 60);
		glVertex2f(-50, 60);
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