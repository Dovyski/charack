#include "move.h"

// Outstading global variables needed by OpenGL ^^
int gPosX = 100;
int gPosY = -100;
int gPosZ = 100;

int gRotX = 0;
int gRotY = 0;

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'w':
			// Move pra frente
			gPosZ += 5;
			break;
		case 's':
			// Move pra tras
			gPosZ -= 5;
			break;
		case 'a':
			// Move para a esquerda
			gPosX += 5;
			break;
		case 'd':
			// Move para a direita
			gPosX -= 5;
			break;

		case 'r':
			// Olha para cima
			gRotX += 1;
			break;
		case 'f':
			// Olha para baixo
			gRotX -= 1;
			break;

		case 'q':
			// Olha para esquerda
			gRotY -= 1;
			break;
		case 'e':
			// Olha para direita
			gRotY += 1;
			break;
	}
}


int getPositionX() {
	return gPosX;
}

int getPositionY() {
	return gPosY;
}

int getPositionZ() {
	return gPosZ;
}