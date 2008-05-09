#ifndef _MOVE_H_
#define _MOVE_H_

#include <windows.h>
#include <stdio.h>

extern int gPosX;
extern int gPosY;
extern int gPosZ;

extern int gRotX;
extern int gRotY;

void processNormalKeys(unsigned char key, int x, int y);
int getPositionX();
int getPositionY();
int getPositionZ();

#endif //_MOVE_H_