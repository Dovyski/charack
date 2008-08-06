#include "CharackWorld.h"

CharackWorld::CharackWorld(int theViewFrustum, int theSample) {
	mCamera  = new CharackObserver();
	mMathsX  = new CharackMathCollection();
	mMathsZ  = new CharackMathCollection();
	
	setViewFrustum(theViewFrustum);
	setSample(theSample);
}

CharackWorld::~CharackWorld() {
}


void CharackWorld::generateMap(void) {
	int aMapX, aMapZ, x, z;
	int aXNow = (int)getObserver()->getPositionX();
	int aZNow = (int)getObserver()->getPositionZ();

	// What we have here is this:
	//
	// aXNow and aZNow hold the value of our current position in the Charack world. To populate
	// mMap with data, we will collect all points localized within the square centered at (aXNow, aZNow) (in the figure
	// below, the square has A, B, C and D as its corners).
	//
	// The size of the square is determinated by getViewFrustum(). We will iterate from A to B (X axis) and, for each step,
	// we will iterare from A to D (or B to C) (Z axis) collecting points.
	//
	// Each of these collected points will be stored at mMap[x][y]. x and y will just be used to index
	// the mMap array, they have no relation with the world coordinates.
	//
	// -------------D------------|------------C    -
	// |            |            |            |    |
	// |            |            |            |    |
	// |------------|-------(aXNow,aZNow)-----| -- | getViewFrustum()
	// |            |            |            |    |
	// |            |            |            |    |
	// -------------A------------|------------B    -
	//        (aMapX, aMapZ)			|
	//									|
	//							 |------------|
	//							 getViewFrustum()/2

	for(aMapX = abs(aXNow) - getViewFrustum()/2, x = 0; x < getViewFrustum(); x++, aMapX+=getSample()){ 
		for(aMapZ = abs(aZNow) - getViewFrustum()/2, z = 0; z < getViewFrustum(); z++, aMapZ+=getSample()){ 
			mMap[x][z] = Vector3(x, getHeight(aMapX, aMapZ), z);
		}
	}
}


void CharackWorld::displayMap(void) {
	Vector3 aNormal;

	glRotatef(getObserver()->getRotationY(), 0,1,0);
	glRotatef(getObserver()->getRotationX(), 1,0,0);

	glTranslatef(-getViewFrustum()/2, -getObserver()->getPosition()->y, -getViewFrustum()/2);

	generateMap();

	aNormal = calculateNormal(mMap[0][1], mMap[0][0], mMap[1][0]);
	glNormal3f(aNormal.x, aNormal.y, aNormal.z);

	glVertex3f(0, mMap[0][0].y,	0);
	glVertex3f(1, mMap[1][0].y,	0);

	for(int x = 0; x < getViewFrustum() - 1; x++){ 
		glBegin(GL_TRIANGLE_STRIP);
		
		for(int z = 1; z < getViewFrustum(); z++){ 
			glVertex3f(x, mMap[x][z].y,	z);

			aNormal = calculateNormal(mMap[x-1][z], mMap[x][z], mMap[x][z-1]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			glVertex3f(mMap[x+1][z].x, mMap[x+1][z].y, mMap[x+1][z].z);
		}
		
		glEnd();

		if((x + 1) < (getViewFrustum() - 1)) {
			aNormal = calculateNormal(mMap[x+1][1], mMap[x+1][0], mMap[x+2][0]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			glVertex3f(mMap[x+1][0].x, mMap[x+1][0].y, mMap[x+1][0].z);
			glVertex3f(mMap[x+2][0].x, mMap[x+2][0].y, mMap[x+2][0].z);
		}
	}
}

float CharackWorld::getHeight(float theX, float theZ) {
	return sin(abs(theX)/(CK_MAX_WIDTH/10)) * 20 + cos(abs(theZ)/(CK_MAX_WIDTH/30)) * 10;
	//return getMathCollectionX()->getValue(abs(theX)/CK_MAX_WIDTH) + getMathCollectionZ()->getValue(abs(theZ)/CK_MAX_WIDTH);
}

CharackObserver *CharackWorld::getObserver(void) {
	return mCamera;
}

CharackMathCollection *CharackWorld::getMathCollectionX(void) {
	return mMathsX;
}

CharackMathCollection *CharackWorld::getMathCollectionZ(void) {
	return mMathsZ;
}

// This method will perform the following: (theLeftPoint - theMiddlePoint) ^ (theRightPoint - theMiddlePoint)
Vector3 CharackWorld::calculateNormal(Vector3 theLeftPoint, Vector3 theMiddlePoint, Vector3 theRightPoint) {
	Vector3 aV0, aV1, aNormal;

	aV0 = theLeftPoint - theMiddlePoint;
	aV1 = theRightPoint - theMiddlePoint;
	aNormal = aV1 ^ aV0;
	aNormal.normalize();

	return aNormal;
}

void CharackWorld::setViewFrustum(int theViewFrustum) {
	mViewFrustum = (theViewFrustum < 0 || theViewFrustum > CK_VIEW_FRUSTUM) ? CK_VIEW_FRUSTUM : theViewFrustum; 
}

int CharackWorld::getViewFrustum() {
	return mViewFrustum;
}

void CharackWorld::printDebugInfo(void) {
	printf("--- Charack World (Debug info) ---\n\n");
	printf("Observer = (%d,%d,%d), [rotx, roty] = (%d, %d)\n", (int)getObserver()->getPositionX(), (int)getObserver()->getPositionY(), (int)getObserver()->getPositionZ(), getObserver()->getRotationX(), getObserver()->getRotationY());	
	printf("View frustum = %d\n", getViewFrustum());
	printf("Sample = %d\n", getSample());
	printf("Terrain height (observer) = %.2f\n", getHeight(getObserver()->getPosition()->x, getObserver()->getPosition()->z));
}

void CharackWorld::setSample(int theSample) {
	mSample = theSample < 0 ? 1 : theSample;
}

int CharackWorld::getSample() {
	return mSample;
}