#include "CharackWorld.h"

CharackWorld::CharackWorld(int theViewFrustum, int theSample) {
	mObserver		= new CharackObserver();
	mCamera			= new CharackCamera();
	mTerrain		= new CharackTerrain();
	mMapGenerator	= new CharackMapGenerator();
	mWorldSlice		= new CharackWorldSlice(this);
	mPerlinNoise	= new Perlin(16, 8, 1, 10);
	mCoastGen		= new CharackCoastGenerator();
	
	// Generate the world. The mMapGenerator is our "guide", it genetares the huge things in the
	// world, like oceans and continents, then the other Charack classes will use that "guide"
	// as a clue repository to generate specific height variation, beach stuff, mountains, etc.
	mMapGenerator->generate();
	
	setViewFrustum(theViewFrustum);
	setSample(theSample);
	setScale(1);

	mTerrain->setCamera(mCamera);
}

CharackWorld::~CharackWorld() {
}

void CharackWorld::render(void) {
	if(getWorldSlice()->updateData()) {
		// The slice has changed. We must update the terrain mesh
		// and all its friends.

		// Before we send the terrain info to the LOD manager, we must edit
		// the info matrix to insert the water information and, after that,
		// make the coast lines a litte bit more realistic.
		// So, lets do that:
		getCoastGenerator()->disturbStraightCoastLines(getWorldSlice()->getHeightData(), getWorldSlice()->getLandAndWaterData());

		// All information we have is smooth and ready to be displayed. 
		// Lets update the LOD manager...
		getTerrain()->build_quad(mWorldSlice->getHeightData());
	}

	getCamera()->render();
	
	glColor3f(1, 1, 1);
	getTerrain()->renderMain();
}


float CharackWorld::getHeight(float theX, float theZ) {
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
				getObserver()->setPosition(x, getObserver()->getPositionY(), z);
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

CharackWorldSlice *CharackWorld::getWorldSlice(void) {
	return mWorldSlice;
}

CharackCoastGenerator *CharackWorld::getCoastGenerator(void) {
	return mCoastGen;
}