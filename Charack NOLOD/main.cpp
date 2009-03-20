/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "CharackTerrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "charack/charack.h"

using namespace std;

float		_angle = 60.0f;
Timer       *timer;
GLFont      *font;
Frames      *frames;

CharackWorld gWorld(300, 1);

void keyboard(unsigned char key, int x, int y)
{
	key = tolower(key);
	switch(key)	{
		case 27:
			exit(0);
			break;

		case 'a':  //seta esquerda
			gWorld.getCamera()->move(1);
			break;

		case 'd': //seta direita
			gWorld.getCamera()->move(-1); //TODO: fix the minus...
			break;

		case 'w': //seta cima
			gWorld.getCamera()->walk(1);
			break;

		case 's': ////seta baixo
			gWorld.getCamera()->walk(-1);
			break;

		case 't': ////pg up
			gWorld.getCamera()->elevate(1);
			break;

		case 'g': ////pg down
			gWorld.getCamera()->elevate(-1);
			break;

		case 'r': ////rotate up
			gWorld.getCamera()->rotate(0.05, 0, 0);
			break;

		case 'f': ////rotate down
			gWorld.getCamera()->rotate(-0.05, 0, 0);
			break;

		case 'q': ////rotate left
			gWorld.getCamera()->rotate(0, -0.01, 0);
			break;

		case 'e': ////rotate right
			gWorld.getCamera()->rotate(0, 0.01, 0);
			break;

		case 'y': // enable top view
			gWorld.getCamera()->topView(1);
			break;

		case 'u': // disable top view
			gWorld.getCamera()->topView(0);
			break;
/*
		case 'i': //wireframe
			if(polygonMode==1) {
				polygonMode=0;
				glPolygonMode(GL_BACK, GL_FILL);
				glPolygonMode(GL_FRONT, GL_FILL);
			} else {
				polygonMode=1;
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			break;
*/

		case 'l':
			gWorld.getCamera()->lockPosition();
			break;

		case 'b': // dumb world slice
			printf("Fazendo dump...\n");
			gWorld.getWorldSlice()->dumpToFile("C:\\Temp\\heightdovyski.raw");
			break;
	}
}

// Numpad stuff
void keyboardSpecial(int key, int x, int y)
{
	int aSpeed	 = 10;
	int aSpeedUp = (int)(gWorld.getSample() * 1.10);

	switch(key)	{
		case GLUT_KEY_UP:
			gWorld.getObserver()->moveForward(aSpeed * aSpeedUp);
			break;

		case GLUT_KEY_DOWN:
			gWorld.getObserver()->moveBackward(aSpeed * aSpeedUp);
			break;

		case GLUT_KEY_RIGHT:
			gWorld.getObserver()->moveRight(aSpeed * aSpeedUp);
			break;

		case GLUT_KEY_LEFT:
			gWorld.getObserver()->moveLeft(aSpeed * aSpeedUp);
			break;

		case GLUT_KEY_PAGE_UP:
			// Increase the sample size
			gWorld.setSample(gWorld.getSample() + gWorld.getSample() * 1.10);
			break;

		case GLUT_KEY_PAGE_DOWN:
			// Decrease the sample size
			gWorld.setSample(gWorld.getSample() - gWorld.getSample()*0.10);
			break;
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);

	frames     = new Frames();
	font       = new GLFont();
	timer      = new Timer();

	gWorld.placeObserverOnLand();
}

void setupLights() {
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void displayHud() {
	static char text[50];
	float fps = frames->getFrames();

	glColor3f (1,0,0);
	font->startText();

	sprintf(text, "Fps: %.2f ", fps);
	font->print(20.0f, 15.0f, text);
	//sprintf(text, "TRI: %d ", gWorld.getTerrain()->triangulos);
	//font->print(20.0f, 45.0f, text);
	sprintf(text, "camPos: (%2.f, %2.f,%2.f)", gWorld.getCamera()->camPos.x, gWorld.getCamera()->camPos.y, gWorld.getCamera()->camPos.z);
	font->print(20.0f, 75.0f, text);
	sprintf(text, "observer: (%2.f, %2.f,%2.f)", gWorld.getObserver()->getPosition()->x, gWorld.getObserver()->getPosition()->y, gWorld.getObserver()->getPosition()->z);
	font->print(20.0f, 105.0f, text);
	sprintf(text, "sample: %d", gWorld.getSample());
	font->print(20.0f, 135.0f, text);
	font->endText();
}


void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(gWorld.getCamera()->camPos.x, gWorld.getCamera()->camPos.y, gWorld.getCamera()->camPos.z);
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(-_angle, 0.0f, 1.0f, 0.0f);
	
	setupLights();
	gWorld.render();
	gWorld.renderReferenceAxis();
	displayHud();
	
	glutPostRedisplay();	
	glutSwapBuffers();
}

void update(int value) {
	_angle += 0.25f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Pseudo-infinite virtual world");
	initRendering();
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecial);
	glutReshapeFunc(handleResize);
	//glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









