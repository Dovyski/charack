#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>

#include "../Charack/perlin.h"

#define CAMERA_DISTANCE		50.0
#define CONTINENT_SIZE		10.0
#define MAX_DIST			15.0
#define NEGATIVE(X)			((X) < 0 ? (X) : ((X)*-1))

int gPontos = 10;
Perlin gPerlinNoise(8, 8, 1, rand());
Perlin gPerlinNoise2(8, 8, 1, rand());

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'o':
			gPontos--;
			break;

		case 'p':
			gPontos++;
			break;
	}
}


void display(void) {
	int i,j, aLastPoint = 0, aPoint;

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 0.0);
	glDisable(GL_POINT_SMOOTH);
	glPointSize(1.0);

	glBegin(GL_LINE_STRIP);
		for(i = 0; i < gPontos; i++) {
			aPoint = gPerlinNoise.Get(i * 0.1, i/gPontos) * 30;
			aPoint = abs(aLastPoint - aPoint) > MAX_DIST ? MAX_DIST : aPoint;

			glVertex2f(aPoint, 20 - i);
			aLastPoint = aPoint;
		}

		for(j = 0; j < gPontos; j++) {
			glVertex2f(NEGATIVE(gPerlinNoise2.Get(j * 0.1, j/gPontos) * 50) - CONTINENT_SIZE, 20 - i + j);
		}

		// Connect the last point with the firts one
		glVertex2f(abs(gPerlinNoise.Get(0, 0) * 50), 20);
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




