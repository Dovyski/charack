#ifndef _CHARACK_RENDER_H_
#define _CHARACK_RENDER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "CharackObserver.h"
#include "CharackMathCollection.h"

// TODO: comment this?
class CharackWorld {
	private:
		CharackObserver *mCamera;
		CharackMathCollection *mMathsX;
		CharackMathCollection *mMathsZ;
		Vector3 mMap[CK_VIEW_FRUSTUM][CK_VIEW_FRUSTUM];
		
		int mViewFrustum;
		int mSample;

		Vector3 calculateNormal(Vector3 theLeftPoint, Vector3 theMiddlePoint, Vector3 theRightPoint);
		void applyColorByHeight(Vector3 thePoint);

	public:
		CharackWorld(int theViewFrustum, int theSample);
		~CharackWorld();

		void displayMap(void);
		float getHeight(float theX, float theZ);
		float getHeightAtObserverPosition(void);
		void generateMap(void);
		CharackObserver *getObserver(void);

		void setViewFrustum(int theViewFrustum);
		int getViewFrustum();

		void setSample(int theViewFrustum);
		int getSample();

		// Print useful information about the world.
		void printDebugInfo(void);
		
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