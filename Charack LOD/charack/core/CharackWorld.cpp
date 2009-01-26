#include "CharackWorld.h"

CharackWorld::CharackWorld(int theViewFrustum, int theSample) {
	mData = (unsigned char *)malloc((DIM_TERRAIN + 1) * (DIM_TERRAIN + 1));

	mObserver		= new CharackObserver();
	mCamera			= new CharackCamera();
	mTerrain		= new CharackTerrain();
	mMapGenerator	= new CharackMapGenerator();
	mPerlinNoise	= new Perlin(16, 8, 1, 10);
	
	// Generate the world. The mMapGenerator is our "guide", it genetares the huge things in the
	// world, like oceans and continents, then the other Charack classes will use that "guide"
	// as a clue repository to generate specific height variation, beach stuff, mountains, etc.
	mMapGenerator->generate();
	
	setViewFrustum(theViewFrustum);
	setSample(theSample);
	setScale(1);

	mTerrain->setCamera(mCamera);

	mOldObserverPos = Vector3(CK_MAX_WIDTH, 0, CK_MAX_WIDTH);
}

CharackWorld::~CharackWorld() {
}


void CharackWorld::generateMap(void) {
	int aMapX, aMapZ, x, z;
	int aXNow = (int)getObserver()->getPositionX();
	int aZNow = (int)getObserver()->getPositionZ();

	// TODO: comment this
	/*
	getMapGenerator()->applyCoast(abs(aXNow) - (getViewFrustum()/2) * getSample(),
								  abs(aZNow) - (getViewFrustum()/2) * getSample(),
								  getViewFrustum(),
								  getSample());*/

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
			if(getMapGenerator()->isLand(aMapX,aMapZ)) {
				mMap[x][z] = Vector3(x, getHeight(aMapX, aMapZ) * normilizeHeight(), z, 1);
			} else {
				mMap[x][z] = Vector3(x, CK_SEA_LEVEL, z, 0);
			}
		}
	}
}

float CharackWorld::normilizeHeight() {
	return getSample() > CK_SAMPLE_CORRECTION_LIMIT ? CK_SAMPLE_CORRECTION : getSample();
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

	applyColorByHeight(mMap[0][0]);	
	glVertex3f(0, mMap[0][0].y,	0);
	
	applyColorByHeight(mMap[1][0]);	
	glVertex3f(1, mMap[1][0].y,	0);

	for(int x = 0; x < getViewFrustum() - 1; x++){ 
		for(int z = 1; z < getViewFrustum(); z++){
			applyColorByHeight(mMap[x][z]);
			glVertex3f(x, mMap[x][z].y,	z);

			aNormal = calculateNormal(mMap[x-1][z], mMap[x][z], mMap[x][z-1]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			applyColorByHeight(mMap[x+1][z]);	
			glVertex3f(mMap[x+1][z].x, mMap[x+1][z].y, mMap[x+1][z].z);
		}
		
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);

		if((x + 1) < (getViewFrustum() - 1)) {
			aNormal = calculateNormal(mMap[x+1][1], mMap[x+1][0], mMap[x+2][0]);
			glNormal3f(aNormal.x, aNormal.y, aNormal.z);

			applyColorByHeight(mMap[x+1][0]);				
			glVertex3f(mMap[x+1][0].x, mMap[x+1][0].y, mMap[x+1][0].z);
			
			applyColorByHeight(mMap[x+2][0]);				
			glVertex3f(mMap[x+2][0].x, mMap[x+2][0].y, mMap[x+2][0].z);
		}
	}
	glEnd();
}

void CharackWorld::render(void) {
	// First of all, we get a slice of the world to render
	generateWorldSlice();

	getCamera()->render();
	
	glColor3f(1, 1, 1);
	getTerrain()->renderMain();
}

void CharackWorld::generateWorldSlice(void) {
	int xMesh, zMesh, aDim = DIM_TERRAIN + 1, i = 0;
	float xObserver = getObserver()->getPositionX(), zObserver = getObserver()->getPositionZ();
	
	// If the observer has changed its location since last update, then we recreate the terrain again, otherwise
	// we can use the old data (which has no modifications).

	if(mOldObserverPos.x != xObserver || mOldObserverPos.z != zObserver) {
		if(xObserver > mOldObserverPos.x) {
			// Moving to the right
			printf("generateWorldSlice: RIGHT\n");
		} else if (xObserver < mOldObserverPos.x) {
			// Moving to the left
			printf("generateWorldSlice: LEFT\n");
		} else if (zObserver > mOldObserverPos.z) {
			// Moving backward
			printf("generateWorldSlice: BACKWARD\n");
		} else if (zObserver < mOldObserverPos.z) {
			// Moving forward
			printf("generateWorldSlice: FORWARD\n");
		}

		for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += getSample()){ 
			for(xMesh = 0, xObserver = getObserver()->getPositionX(); xMesh < aDim; xMesh++, xObserver += getSample()){ 
				mData[i++] = (char)getHeight(xObserver, zObserver);
			}
		}
		getTerrain()->build_quad(mData);
		
		printf("limpando cache old:(%.2f,%.2f), pos:(%.2f,%.2f)...\n", mOldObserverPos.x, mOldObserverPos.z, getObserver()->getPositionX(), getObserver()->getPositionZ());
		mOldObserverPos.x = getObserver()->getPositionX();
		mOldObserverPos.z = getObserver()->getPositionZ();
	}
}

void CharackWorld::applyColorByHeight(Vector3 thePoint) {
	float aRand = (1.0 * (rand() / (RAND_MAX + 1.0)));

	// The "alpha" value tell us if the point is land or not.
	if(!thePoint.a) {
		glColor3f(0.0f, 0.0f, 0.5f);
	} else {
		//glColor3f(thePoint.y/500, (200 - thePoint.y)/3000, 0.0f);
		//if(aRand < 0.5) {
		//	printf("%.8f\n", abs(thePoint.x/(CK_MAX_WIDTH/2000)));
		//}
		glColor3f(0.0f, abs(mPerlinNoise->Get(0.22, abs(thePoint.y)))*0.7, 0.0f);
	}
}

float CharackWorld::getHeight(float theX, float theZ) {
	//return mHeightFunctionX(theX) + mHeightFunctionZ(theZ);
	return mPerlinNoise->Get(theX/2000, theZ/2000) * 200;
}

float CharackWorld::getHeightAtObserverPosition(void) {
	return getHeight(getObserver()->getPosition()->x, getObserver()->getPosition()->z);
}

CharackObserver *CharackWorld::getObserver(void) {
	return mObserver;
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


void CharackWorld::setHeightFunctionX(float (*theFunction)(float)) {
	mHeightFunctionX = theFunction;
}

void CharackWorld::setHeightFunctionZ(float (*theFunction)(float)) {
	mHeightFunctionZ = theFunction;
}

CharackCamera *CharackWorld::getCamera(void) {
	return mCamera;
}

CharackTerrain *CharackWorld::getTerrain(void) {
	return mTerrain;
}
