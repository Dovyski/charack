#ifdef _WIN32
	#include <windows.h>
#endif

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "charack/charack.h"

//variaveis globais
Timer       *timer;
GLFont      *font;
Frames      *frames;
int         polygonMode=1;

CharackWorld gWorld(300, 1);

void setupLights(void) {
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
}


//faz a geracao da cena. Esta funcao e' chamada apenas no inicio da execucao.
void init()
{
	frames     = new Frames();
	font       = new GLFont();
	timer      = new Timer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);

	//gWorld.placeObserverOnLand();
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


////////////////////////////////////////////////////////////////////////////////////////
void display (void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -10.0f);
	//glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

	setupLights();
	//gWorld.render();

	float scale = 5.0f / max(gWorld.getTerrain()->width() - 1, gWorld.getTerrain()->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(gWorld.getTerrain()->width() - 1) / 2,
				 0.0f,
				 -(float)(gWorld.getTerrain()->length() - 1) / 2);
	
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < gWorld.getTerrain()->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < gWorld.getTerrain()->width(); x++) {
			Vec3f normal = gWorld.getTerrain()->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glColor3f(0.5f, 0.0f, 0.0f);
			glVertex3f(x, gWorld.getTerrain()->getHeight(x, z), z);
			//printf("(%d, %.2f, %d), ", x, gWorld.getTerrain()->getHeight(x, z), z);
			normal = gWorld.getTerrain()->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glColor3f(0.5f, 0.0f, 0.0f);
			glVertex3f(x, gWorld.getTerrain()->getHeight(x, z + 1), z + 1);
			//printf("(%d, %.2f, %d), ", x, gWorld.getTerrain()->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}


	//displayHud();
	
	glutSwapBuffers();
}


//faz a leitura da entrada do usuario
void keyboard(unsigned char key, int x, int y)
{
	key = tolower(key);
	switch(key)	{
		case 27:
			exit(0);
			break;

		case 'a':  //seta esquerda
			gWorld.getCamera()->move(500);
			break;

		case 'd': //seta direita
			gWorld.getCamera()->move(-500); //TODO: fix the minus...
			break;

		case 'w': //seta cima
			gWorld.getCamera()->walk(500);
			break;

		case 's': ////seta baixo
			gWorld.getCamera()->walk(-500);
			break;

		case 't': ////pg up
			gWorld.getCamera()->elevate(200);
			break;

		case 'g': ////pg down
			gWorld.getCamera()->elevate(-200);
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
		case '-': 
			gWorld.getTerrain()->ThresholdDetail+=0.1;
			if( gWorld.getTerrain()->ThresholdDetail > 170 ) {
				gWorld.getTerrain()->ThresholdDetail = 170;
			}
			printf("\nThresholdDetail = %.2f",    gWorld.getTerrain()->ThresholdDetail);
			break;

		case '+': 
			gWorld.getTerrain()->ThresholdDetail-=0.1;
			if( gWorld.getTerrain()->ThresholdDetail < 0.01 ) {
				gWorld.getTerrain()->ThresholdDetail = 0.01;
			}
			printf("\nThresholdDetail = %.2f",    gWorld.getTerrain()->ThresholdDetail);
			break;
*/

		case 'i': //wireframe
			if(polygonMode==1) {
				polygonMode=0;
				//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glPolygonMode(GL_BACK, GL_FILL);
				glPolygonMode(GL_FRONT, GL_FILL);
			} else {
				polygonMode=1;
				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			break;

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
	int aSpeedUp = (int)(gWorld.getSample() * 1.10);

	switch(key)	{
		case GLUT_KEY_UP:
			gWorld.getObserver()->moveForward(10 * aSpeedUp);
			break;

		case GLUT_KEY_DOWN:
			gWorld.getObserver()->moveBackward(10 * aSpeedUp);
			break;

		case GLUT_KEY_RIGHT:
			gWorld.getObserver()->moveRight(10 * aSpeedUp);
			break;

		case GLUT_KEY_LEFT:
			gWorld.getObserver()->moveLeft(10 * aSpeedUp);
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


////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
   glutInitWindowSize (SCREEN_X, SCREEN_Y);
   glutInitWindowPosition (350, 100);
   glutCreateWindow ("Pseudo-infinite virtual world");

//	glutGameModeString( "990x768:32@75" ); //the settings for fullscreen mode
//	glutEnterGameMode(); //set glut to fullscreen using the settings in the line above

   init();

   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(keyboardSpecial);
//   glutPassiveMotionFunc(passiveMotionFunc);
   //glutMotionFunc(MotionFunc);
   glutIdleFunc(display);

   glutMainLoop();
   return 0;
}