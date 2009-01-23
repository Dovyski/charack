#ifndef _CHARACK_RENDER_H_
#define _CHARACK_RENDER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "CharackObserver.h"
#include "CharackMapGenerator.h"
#include "CharackCamera.h"
#include "CharackTerrain.h"

#include "../tools/perlin.h"

// TODO: comment this?
class CharackWorld {
	private:
		CharackObserver *mObserver;
		CharackMapGenerator *mMapGenerator;
		CharackCamera *mCamera;
		CharackTerrain *mTerrain;

		Vector3 mMap[CK_VIEW_FRUSTUM][CK_VIEW_FRUSTUM];
		float (*mHeightFunctionX)(float); // generate the height coordinates for X axis
		float (*mHeightFunctionZ)(float); // generate the height coordinates for X axis
		
		int mViewFrustum;
		int mSample;
		float mScale;
		Perlin *mPerlinNoise;
		unsigned char *mData;

		Vector3 calculateNormal(Vector3 theLeftPoint, Vector3 theMiddlePoint, Vector3 theRightPoint);
		void applyColorByHeight(Vector3 thePoint);
		float normilizeHeight();

	public:
		CharackWorld(int theViewFrustum, int theSample);
		~CharackWorld();

		void displayMap(void);
		float getHeight(float theX, float theZ);
		float getHeightAtObserverPosition(void);
		void generateMap(void);
		CharackObserver *getObserver(void);
		CharackMapGenerator *getMapGenerator(void);
		CharackCamera *getCamera(void);
		CharackTerrain *getTerrain(void);

		void setViewFrustum(int theViewFrustum);
		int getViewFrustum();

		void setSample(int theViewFrustum);
		int getSample();

		void setScale(float theScale);
		float getScale();

		void setHeightFunctionX(float (*theFunction)(float));
		void setHeightFunctionZ(float (*theFunction)(float));

		// Print useful information about the world.
		void printDebugInfo(void);

		void placeObserverOnLand(void);
		void render(void);
};

// Useful datatypes we can use in the render processing.
typedef struct {
	float x; 
	float y;
	float z;
} CK_NORMALS;

#endif