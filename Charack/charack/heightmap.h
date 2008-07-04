#ifndef _HEIGHT_MAP_H_
#define _HEIGHT_MAP_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "CharackCamera.h"

void LoadHeightMap (char* Filename, int Width, int Height);

void DisplayHeightMap (void);

void displayHeightMapDovyski(void);

BYTE getHeight(float theX, float theZ);

#endif