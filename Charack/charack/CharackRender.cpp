#include "CharackRender.h"

CharackRender::CharackRender() {
	mCamera  = new CharackCamera();
	mMathsX  = new CharackMathCollection();
	mMathsZ  = new CharackMathCollection();
}

CharackRender::~CharackRender() {
}


void CharackRender::generateMap(void) {
	int mapX, mapZ, x, z;
	int aXNow = (int)getCamera()->getPositionX();
	int aZNow = (int)getCamera()->getPositionZ();

	mapX = abs(aXNow) - CK_VIEW_FRUSTUM;
	mapZ = abs(aZNow) - CK_VIEW_FRUSTUM;

	for(x = 0; x < CK_VIEW_FRUSTUM; x++){ 
		for(mapZ = 0, z = 0; z < CK_VIEW_FRUSTUM; z++){ 
			mHeightMap[x][z] = getHeight(mapX, mapZ);
			mapZ++;
		}
		mapX++;
	}
}


void CharackRender::displayMap(void) {
	glRotatef(mCamera->getRotationY(), 0,1,0);
	glRotatef(getCamera()->getRotationX(), 1,0,0);
	glTranslatef(-CK_VIEW_FRUSTUM/2, getCamera()->getPosition()->y, -CK_VIEW_FRUSTUM/2);

	generateMap();
/*
	printf("x=%d,z=%d\n", aXNow, aZNow);

	for(int x = 0; x < CK_VIEW_FRUSTUM*2; x++){ 
		glBegin(GL_TRIANGLE_STRIP);
		
		for(int z = 0; z < CK_VIEW_FRUSTUM*2; z++){ 
			if(z < 3) printf("x=%d,y=%d, z=%d\n", aXNow + x, mHeightMap[x][z], abs(aZNow) - CK_VIEW_FRUSTUM + z);
			glVertex3f(abs(aXNow) - CK_VIEW_FRUSTUM  + x,	mHeightMap[x][z],	abs(aZNow) - CK_VIEW_FRUSTUM + z);
			glVertex3f(abs(aXNow) - CK_VIEW_FRUSTUM + 4 + x,	mHeightMap[x + 1][z],	abs(aZNow) - CK_VIEW_FRUSTUM + z);
		}
		glEnd();
	}*/

	for(int x = 0; x < CK_VIEW_FRUSTUM - 1; x++){ 
		glBegin(GL_TRIANGLE_STRIP);
		for(int z = 0; z < CK_VIEW_FRUSTUM; z++){ 
			glVertex3f(x,	mHeightMap[x][z],	z);
			glVertex3f(x + 1,	mHeightMap[x+1][z],	z);
		}
		glEnd();
	}
}

float CharackRender::getHeight(float theX, float theZ) {
	return sin(abs(theX)/(CK_MAX_WIDTH/20)) * 20 + cos(abs(theZ)/(CK_MAX_WIDTH/40)) * 10;
	//return getMathCollectionX()->getValue(abs(theX)/CK_MAX_WIDTH) + getMathCollectionZ()->getValue(abs(theZ)/CK_MAX_WIDTH);
}

CharackCamera *CharackRender::getCamera(void) {
	return mCamera;
}

CharackMathCollection *CharackRender::getMathCollectionX(void) {
	return mMathsX;
}

CharackMathCollection *CharackRender::getMathCollectionZ(void) {
	return mMathsZ;
}