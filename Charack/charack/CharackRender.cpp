#include "CharackRender.h"

CharackRender::CharackRender() {
	mCamera  = new CharackCamera();
	mMathsX  = new CharackMathCollection();
	mMathsZ  = new CharackMathCollection();
}

CharackRender::~CharackRender() {
}

void CharackRender::displayMap(void) {
	int mapX, mapZ;
	float aHeight;
	int aXNow = (int)getCamera()->getPositionX();
	int aZNow = (int)getCamera()->getPositionZ();

	glRotatef(mCamera->getRotationY(), 0,1,0);
	glRotatef(getCamera()->getRotationX(), 1,0,0);
	glTranslatef(getCamera()->getPosition()->x, getCamera()->getPosition()->y, getCamera()->getPosition()->z);

	printf("x=%d,z=%d, threshold=%f\n", aXNow, aZNow, (abs(aXNow)/CK_MAX_WIDTH) * 2*PI);



	// Rendering the ground
	for (mapX = abs(aXNow) - CK_VIEW_FRUSTUM; mapX < abs(aXNow) + CK_VIEW_FRUSTUM; mapX +=4){ 
		glBegin(GL_TRIANGLE_STRIP);

		for (mapZ = abs(aZNow) - CK_VIEW_FRUSTUM; mapZ < abs(aZNow) + CK_VIEW_FRUSTUM; mapZ+=4){ 


/*
			glBegin(GL_TRIANGLE);
				aHeight = getHeight(mapX, mapZ);
				glVertex3f(mapX,aHeight,mapZ);
				
				aHeight = getHeight(mapX, mapZ+4);
				glVertex3f(mapX,aHeight,mapZ+4);

				aHeight = getHeight(mapX+4, mapZ);
				glVertex3f(mapX+4,aHeight,mapZ);

				aHeight = getHeight(mapX+4, mapZ+4);
				glVertex3f(mapX,aHeight,mapZ+4);
*/

				aHeight = getHeight(mapX, mapZ);
				glVertex3f(mapX,aHeight,mapZ);
				
				aHeight = getHeight(mapX+4, mapZ);
				glVertex3f(mapX+4,aHeight,mapZ);

				//aHeight = getHeight(mapX+4, mapZ);
				//glVertex3f(mapX+4,aHeight,mapZ);

				//aHeight = getHeight(mapX+4, mapZ+4);
				//glVertex3f(mapX,aHeight,mapZ+4);
		}
		glEnd();
	}

	// Rendering the very first piece of water of our word
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