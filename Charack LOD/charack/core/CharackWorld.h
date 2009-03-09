#ifndef _CHARACK_RENDER_H_
#define _CHARACK_RENDER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

class CharackWorldSlice;

#include "CharackObserver.h"
#include "CharackMapGenerator.h"
#include "CharackCamera.h"
#include "CharackTerrain.h"
#include "CharackWorldSlice.h"

#include "../tools/perlin.h"

// TODO: comment this?
class CharackWorld {
	private:
		CharackObserver *mObserver;
		CharackMapGenerator *mMapGenerator;
		CharackCamera *mCamera;
		CharackTerrain *mTerrain;
		CharackWorldSlice *mWorldSlice;
		CharackCoastGenerator *mCoastGen;

		float (*mHeightFunctionX)(float); // generate the height coordinates for X axis
		float (*mHeightFunctionZ)(float); // generate the height coordinates for X axis
		
		int mViewFrustum;
		int mSample;
		float mScale;
		Perlin *mPerlinNoise;

		Vector3 calculateNormal(Vector3 theLeftPoint, Vector3 theMiddlePoint, Vector3 theRightPoint);

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
		CharackCoastGenerator *getCoastGenerator(void);

		void setViewFrustum(int theViewFrustum);
		int getViewFrustum();

		void setSample(int theViewFrustum);
		int getSample();

		void setScale(float theScale);
		float getScale();

		CharackWorldSlice *getWorldSlice(void);

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