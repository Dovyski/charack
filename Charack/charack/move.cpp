#include "move.h"

void processNormalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'w':
			// Move forward
			gCamera.moveForward();
			/*
			gCameraPosition.z = gCameraPosition.z + CK_CAMERA_MOV_SPEED*cos(DEG2RAD(360 - gRotY));
			gCameraPosition.x = gCameraPosition.x + CK_CAMERA_MOV_SPEED*sin(DEG2RAD(360 - gRotY));
			*/
			break;
		case 's':
			// Move backward
			gCamera.moveBackward();
			/*
			gCameraPosition.z = gCameraPosition.z - CK_CAMERA_MOV_SPEED*cos(DEG2RAD(360 - gRotY));
			gCameraPosition.x = gCameraPosition.x - CK_CAMERA_MOV_SPEED*sin(DEG2RAD(360 - gRotY));
			*/
			break;
		case 'a':
			// Move left
			gCamera.moveLeft();
			/*
			gCameraPosition.z = gCameraPosition.z + CK_CAMERA_MOV_SPEED*cos(DEG2RAD(90 - gRotY));
			gCameraPosition.x = gCameraPosition.x + CK_CAMERA_MOV_SPEED*sin(DEG2RAD(90 - gRotY));
			*/
			break;
		case 'd':
			// Move right
			gCamera.moveRight();
			/*
			gCameraPosition.z = gCameraPosition.z + CK_CAMERA_MOV_SPEED*cos(DEG2RAD(270 - gRotY));
			gCameraPosition.x = gCameraPosition.x + CK_CAMERA_MOV_SPEED*sin(DEG2RAD(270 - gRotY));
			*/
			break;

		case 'r':
			// Olha para cima
			gCamera.rotateLookUpDown(5);

			//gRotX += CK_CAMERA_ROT_SPEED;
			break;
		case 'f':
			// Olha para baixo
			gCamera.rotateLookUpDown(-5);

			//gRotX -= CK_CAMERA_ROT_SPEED;
			break;

		case 'q':
			// Olha para esquerda
			gCamera.rotateLookLeftRight(-5);


			//gRotY -= CK_CAMERA_ROT_SPEED;
			break;
		case 'e':
			// Olha para direita
			gCamera.rotateLookLeftRight(5);
			//gRotY += CK_CAMERA_ROT_SPEED;
			break;
	}
}
