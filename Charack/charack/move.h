#ifndef _MOVE_H_
#define _MOVE_H_

#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "vector3.h"
#include "config.h"

extern int gRotX;
extern int gRotY;

extern Vector3 gCameraPosition;

void processNormalKeys(unsigned char key, int x, int y);
int getPositionX();
int getPositionY();
int getPositionZ();

#endif //_MOVE_H_