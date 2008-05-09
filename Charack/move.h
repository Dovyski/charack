#ifndef _MOVE_H_
#define _MOVE_H_

#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "vector3.h"
#include "config.h"

#define DEG2RAD(X)		((PI*(X))/180)

extern int gPosX;
extern int gPosY;
extern int gPosZ;

extern int gRotX;
extern int gRotY;

extern Vector3 gCameraPosition;

void processNormalKeys(unsigned char key, int x, int y);
int getPositionX();
int getPositionY();
int getPositionZ();

#endif //_MOVE_H_