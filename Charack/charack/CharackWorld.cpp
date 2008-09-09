#include "CharackWorld.h"

CharackWorld::CharackWorld(int theViewFrustum, int theSample) {
	mCamera			= new CharackObserver();
	mMathsX			= new CharackMathCollection();
	mMathsZ			= new CharackMathCollection();
	mMapGenerator	= new CharackMapGenerator();
	
	// Generate the world. The mMapGenerator is our "guide", it genetares the huge things in the
	// world, like oceans and continents, then the other Charack classes will use that "guide"
	// as a clue repository to generate specific height variation, beach stuff, mountains, etc.
	mMapGenerator->generate();
	
	setViewFrustum(theViewFrustum);
	setSample(theSample);
	setScale(1);
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

	for(aMapX = abs(aXNow) - (getViewFrustum()/2) * getSample(), x = 0; x < getViewFrustum(); x++, aMapX+=getSample()){ 
		for(aMapZ = abs(aZNow) - (getViewFrustum()/2) * getSample(), z = 0; z < getViewFrustum(); z++, aMapZ+=getSample()){ 
			mMap[x][z] = Vector3(x, getHeight(aMapX, aMapZ), z);
		}
	}
}


void CharackWorld::displayMap(void) {
	Vector3 aNormal;
	int aHalfViewFrustum = getViewFrustum()/2;

	glRotatef(getObserver()->getRotationY(), 0,1,0);
	glRotatef(getObserver()->getRotationX(), 1,0,0);

	glScalef(getScale(), getScale(), getScale());

	glTranslatef(-aHalfViewFrustum, -getObserver()->getPosition()->y, -aHalfViewFrustum);

	generateMap();

	glBegin(GL_TRIANGLE_STRIP);

	aNormal = calculateNormal(mMap[0][1], mMap[0][0], mMap[1][0]);
	glNormal3f(aNormal.x, aNormal.y, aNormal.z);

	glVertex3f(0, mMap[0][0].y,	0);
	glVertex3f(1, mMap[1][0].y,	0);

	for(int x = 0; x < getViewFrustum() - 1; x++){ 
		for(int z = 1; z < getViewFrustum(); z++){
			applyColorByHeight(mMap[x][z]);
			glVertex3f(x, mMap[x][z].y,	z);

			aNormal = calculateNormal(mMap[x-1][z], mMap[x][z], mMap[x][z-1]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			glVertex3f(mMap[x+1][z].x, mMap[x+1][z].y, mMap[x+1][z].z);
		}
		
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);

		if((x + 1) < (getViewFrustum() - 1)) {
			aNormal = calculateNormal(mMap[x+1][1], mMap[x+1][0], mMap[x+2][0]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			glVertex3f(mMap[x+1][0].x, mMap[x+1][0].y, mMap[x+1][0].z);
			glVertex3f(mMap[x+2][0].x, mMap[x+2][0].y, mMap[x+2][0].z);
		}
	}
	glEnd();
}

void CharackWorld::applyColorByHeight(Vector3 thePoint) {
	glColor3f(thePoint.y/500, (200 - thePoint.y)/3000, 0.0f);
}

float CharackWorld::getHeight(float theX, float theZ) {
	return getMapGenerator()->isLand(theX,theZ) ? getMathCollectionX()->getValue(abs(theX)) + getMathCollectionZ()->getValue(abs(theZ)) : 1;
}

float CharackWorld::getHeightAtObserverPosition(void) {
	return getHeight(getObserver()->getPosition()->x, getObserver()->getPosition()->z);
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
	printf("Scale = %.2f\n", getScale());
	printf("Terrain height (observer) = %.2f\n", getHeightAtObserverPosition());
	printf("isLand: %d\n", getMapGenerator()->isLand(getObserver()->getPositionX(), getObserver()->getPositionZ()));
	printf("\nControls:\n");
	printf("\t Move: w,a,s,d\n");
	printf("\t Turn: q,e\n");
	printf("\t Curve: r,f\n");
	printf("\t Move Up/Down: t,g\n");
	printf("\t View frustum: c,v\n");
	printf("\t Sampling: n,m\n");
	printf("\t Scale: k,l\n");
}

void CharackWorld::placeObserverOnLand() {
	for(int z=0; z < CK_MAX_WIDTH; z++) {
		for(int x=0; x < CK_MAX_WIDTH; x++) {
			if(getMapGenerator()->isLand(x, z)) {
				getObserver()->setPosition(-x, getObserver()->getPositionY(), -z);
				return;
			}
		}
	}
}


void CharackWorld::setSample(int theSample) {
	mSample = theSample < 0 ? 1 : theSample;
}

int CharackWorld::getSample() {
	return mSample;
}

void CharackWorld::setScale(float theScale) {
	mScale = theScale;
}

float CharackWorld::getScale() {
	return mScale;
}

CharackMapGenerator *CharackWorld::getMapGenerator(void) {
	return mMapGenerator;
}