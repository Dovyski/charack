#ifndef _CHARACK_RENDER_H_
#define _CHARACK_RENDER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "CharackCamera.h"
#include "CharackMathCollection.h"

// TODO: comment this?
class CharackRender {
	private:
		CharackCamera *mCamera;
		CharackMathCollection *mMathsX;
		CharackMathCollection *mMathsZ;
		float mHeightMap[CK_VIEW_FRUSTUM*2][CK_VIEW_FRUSTUM*2];

	public:
		CharackRender();		
		~CharackRender();

		void loadHeightMap(char* Filename, int Width, int Height);
		void displayMap(void);
		float getHeight(float theX, float theZ);
		void generateMap(void);
		CharackCamera *getCamera(void);
		
		// TODO: fix this ("ordinary" users should not see the math collection...)
		CharackMathCollection *getMathCollectionX(void);
		CharackMathCollection *getMathCollectionZ(void);
};

// Useful datatypes we can use in the render processing.
typedef struct {
	float x; 
	float y;
	float z;
} CK_NORMALS;

#endif