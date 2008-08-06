#include "CharackRender.h"

CharackRender::CharackRender() {
	mCamera  = new CharackCamera();
	mMathsX  = new CharackMathCollection();
	mMathsZ  = new CharackMathCollection();
}

CharackRender::~CharackRender() {
}


void CharackRender::generateMap(void) {
	int aMapX, aMapZ, x, z;
	int aXNow = (int)getCamera()->getPositionX();
	int aZNow = (int)getCamera()->getPositionZ();

	system("cls");
	printf("(x,y,z) = (%d,%d,%d), (rotx, roty) = (%d, %d)\n", (int)getCamera()->getPositionX(), (int)getCamera()->getPositionY(), (int)getCamera()->getPositionZ(), getCamera()->getRotationX(), getCamera()->getRotationY());

	// What we have here is this:
	//
	// aXNow and aZNow hold the value of our current position in the Charack world. To populate
	// mMap with data, we will collect all points localized within the square centered at (aXNow, aZNow) (in the figure
	// below, the square has A, B, C and D as its corners).
	//
	// The size of the square is determinated by CK_VIEW_FRUSTUM. We will iterate from A to B (X axis) and, for each step,
	// we will iterare from A to D (or B to C) (Z axis) collecting points.
	//
	// Each of these collected points will be stored at mMap[x][y]. x and y will just be used to index
	// the mMap array, they have no relation with the world coordinates.
	//
	// -------------D------------|------------C    -
	// |            |            |            |    |
	// |            |            |            |    |
	// |------------|-------(aXNow,aZNow)-----| -- | CK_VIEW_FRUSTUM
	// |            |            |            |    |
	// |            |            |            |    |
	// -------------A------------|------------B    -
	//        (aMapX, aMapZ)			|
	//									|
	//							 |------------|
	//							 CK_VIEW_FRUSTUM/2

	for(aMapX = abs(aXNow) - CK_VIEW_FRUSTUM/2, x = 0; x < CK_VIEW_FRUSTUM; x++, aMapX++){ 
		for(aMapZ = abs(aZNow) - CK_VIEW_FRUSTUM/2, z = 0; z < CK_VIEW_FRUSTUM; z++, aMapZ++){ 
			mMap[x][z] = Vector3(x, getHeight(aMapX, aMapZ), z);
		}
	}
}


void CharackRender::displayMap(void) {
	Vector3 aNormal;

	glRotatef(getCamera()->getRotationY(), 0,1,0);
	glRotatef(getCamera()->getRotationX(), 1,0,0);

	glTranslatef(-CK_VIEW_FRUSTUM/2, -getCamera()->getPosition()->y, -CK_VIEW_FRUSTUM/2);

	generateMap();

	aNormal = calculateNormal(mMap[0][1], mMap[0][0], mMap[1][0]);
	glNormal3f(aNormal.x, aNormal.y, aNormal.z);

	glVertex3f(0, mMap[0][0].y,	0);
	glVertex3f(1, mMap[1][0].y,	0);

	for(int x = 0; x < CK_VIEW_FRUSTUM - 1; x++){ 
		glBegin(GL_TRIANGLE_STRIP);
		
		for(int z = 1; z < CK_VIEW_FRUSTUM; z++){ 
			glVertex3f(x, mMap[x][z].y,	z);

			aNormal = calculateNormal(mMap[x-1][z], mMap[x][z], mMap[x][z-1]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			glVertex3f(mMap[x+1][z].x, mMap[x+1][z].y, mMap[x+1][z].z);
		}
		
		glEnd();

		if((x + 1) < (CK_VIEW_FRUSTUM - 1)) {
			aNormal = calculateNormal(mMap[x+1][1], mMap[x+1][0], mMap[x+2][0]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			glVertex3f(mMap[x+1][0].x, mMap[x+1][0].y, mMap[x+1][0].z);
			glVertex3f(mMap[x+2][0].x, mMap[x+2][0].y, mMap[x+2][0].z);
		}
	}
}

float CharackRender::getHeight(float theX, float theZ) {
	return sin(abs(theX)/(CK_MAX_WIDTH/20)) * 20 + cos(abs(theZ)/(CK_MAX_WIDTH/30)) * 10;
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

// This method will perform the following: (theLeftPoint - theMiddlePoint) ^ (theRightPoint - theMiddlePoint)
Vector3 CharackRender::calculateNormal(Vector3 theLeftPoint, Vector3 theMiddlePoint, Vector3 theRightPoint) {
	Vector3 aV0, aV1, aNormal;

	aV0 = theLeftPoint - theMiddlePoint;
	aV1 = theRightPoint - theMiddlePoint;
	aNormal = aV1 ^ aV0;
	aNormal.normalize();

	return aNormal;
}