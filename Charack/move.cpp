#include "move.h"

// Outstading global variables needed by OpenGL ^^
// TODO: some raining day, get all this numbers from config.h or something like that.
int gPosX = 100;
int gPosY = -100;
int gPosZ = 100;

int gRotX = 0;
int gRotY = 0;

// TODO: some raining day, get all this numbers from config.h or something like that.
Vector3 gCameraPosition(100, -100, 100);

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'w':
			// Move forward
			gCameraPosition.z = gCameraPosition.z + CK_CAMERA_MOV_SPEED*cos(DEG2RAD(360 - gRotY));
			gCameraPosition.x = gCameraPosition.x + CK_CAMERA_MOV_SPEED*sin(DEG2RAD(360 - gRotY));
			break;
		case 's':
			// Move backward
			gCameraPosition.z = gCameraPosition.z - CK_CAMERA_MOV_SPEED*cos(DEG2RAD(360 - gRotY));
			gCameraPosition.x = gCameraPosition.x - CK_CAMERA_MOV_SPEED*sin(DEG2RAD(360 - gRotY));
			break;
		case 'a':
			// Move left
			gCameraPosition.z = gCameraPosition.z + CK_CAMERA_MOV_SPEED*cos(DEG2RAD(90 - gRotY));
			gCameraPosition.x = gCameraPosition.x + CK_CAMERA_MOV_SPEED*sin(DEG2RAD(90 - gRotY));
			break;
		case 'd':
			// Move right
			gCameraPosition.z = gCameraPosition.z + CK_CAMERA_MOV_SPEED*cos(DEG2RAD(270 - gRotY));
			gCameraPosition.x = gCameraPosition.x + CK_CAMERA_MOV_SPEED*sin(DEG2RAD(270 - gRotY));
			break;

		case 'r':
			// Olha para cima
			gRotX += CK_CAMERA_ROT_SPEED;
			break;
		case 'f':
			// Olha para baixo
			gRotX -= CK_CAMERA_ROT_SPEED;
			break;

		case 'q':
			// Olha para esquerda
			gRotY -= CK_CAMERA_ROT_SPEED;
			break;
		case 'e':
			// Olha para direita
			gRotY += CK_CAMERA_ROT_SPEED;
			break;
	}
}


int getPositionX() {
	return (int)gCameraPosition.x;
}

int getPositionY() {
	return (int)gCameraPosition.y;
}

int getPositionZ() {
	return (int)gCameraPosition.z;
}