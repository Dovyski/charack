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
bool		texture = false;
bool		cullingB = false;

CharackWorld gWorld(300, 1);

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


//faz a geracao da cena. Esta funcao e' chamada apenas no inicio da execucao.
void init()
{
	frames     = new Frames();
	font       = new GLFont();
	timer      = new Timer();

	//inicializacao do OpenGL
	glClearColor(0, 0, 0, 1);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT, GL_LINE);


	glVertexPointer (3, GL_FLOAT, 0, gWorld.getTerrain()->coordVert);     //coord per vertex, coord type, offset, pointer 
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	float escala = 1.0 / (DIM_TERRAIN*STEP); 
	float	p[4] = { escala, 0, 0, 0 };
	glTexGenfv(GL_S, GL_OBJECT_PLANE, p);


	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	p[0] = 0;	p[2] = escala;
	glTexGenfv(GL_T, GL_OBJECT_PLANE, p);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glEnable(GL_TEXTURE_2D);

	gWorld.placeObserverOnLand();
}


////////////////////////////////////////////////////////////////////////////////////////
void display (void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);		
	glEnable(GL_NORMALIZE);		
	glEnable(GL_COLOR_MATERIAL); 
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	setupLights();

	gWorld.render();

	static char text[50];
	float fps = frames->getFrames();

	glColor3f (1,0,0);
	font->startText();
	//glColor3f(1,1,1);
	sprintf(text, "Fps: %.2f ", fps);
	font->print(20.0f, 15.0f, text);
	sprintf(text, "TRI: %d ", gWorld.getTerrain()->triangulos);
	font->print(20.0f, 45.0f, text);
	sprintf(text, "camPos: (%2.f, %2.f,%2.f)", gWorld.getCamera()->camPos.x, gWorld.getCamera()->camPos.y, gWorld.getCamera()->camPos.z);
	font->print(20.0f, 75.0f, text);
	sprintf(text, "observer: (%2.f, %2.f,%2.f)", gWorld.getObserver()->getPosition()->x, gWorld.getObserver()->getPosition()->y, gWorld.getObserver()->getPosition()->z);
	font->print(20.0f, 105.0f, text);
	sprintf(text, "sample: %d", gWorld.getSample());
	font->print(20.0f, 135.0f, text);
	font->endText();
	
	//   TriangleCounter += gWorld.getTerrain()->triangulos;
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

		case 'k': //texture
			if(texture==true)
				texture=false;
			else
				texture=true;
			break;

		case 'j': //culling
			if(cullingB==true)
				cullingB=false;
			else
				cullingB=true;
			break;

		case 'b': // dumb world slice
			printf("Fazendo dump...\n");
			gWorld.getWorldSlice()->dumpToFile("C:\\Temp\\dump-world-slice.txt");
			break;
	}
}  

// Numpad stuff
void keyboardSpecial(int key, int x, int y)
{
	int aSpeedUp	= (int)(gWorld.getSample() * 1.10);

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