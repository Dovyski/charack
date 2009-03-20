#include "CharackCoastGenerator.h"

CharackCoastGenerator::CharackCoastGenerator() {
	mMaxSteps = CK_COAST_MAX_STEP;
	mMaxBeachHeight = CK_COAST_BEACH_HEIGHT;
}

CharackCoastGenerator::~CharackCoastGenerator() {
}

void CharackCoastGenerator::setMaxSteps(int theValue) {
	mMaxSteps = theValue <= 0 ? 0 : theValue;
}

int CharackCoastGenerator::getMaxSteps() {
	return mMaxSteps;
}

void CharackCoastGenerator::setMaxBeachHeight(int theValue) {
	mMaxBeachHeight = theValue <= 0 ? 0 : theValue;
}

int CharackCoastGenerator::getMaxBeachHeight() {
	return mMaxBeachHeight;
}

void CharackCoastGenerator::disturbStraightCoastLines(unsigned char *theHeightData, CharackMapGenerator *theMapGenerator, CharackObserver *theObserver, int theSample) {
	int aDim = CK_DIM_TERRAIN, i = 0, aDistanceLeft, aDistanceRight, aDistanceUp, aDistanceDown, xMesh,zMesh;
	unsigned char aHeight = 0;
	float xObserver = theObserver->getPositionX(), zObserver = theObserver->getPositionZ(), aBeachHeight = 0, aTotalDistance = 0;

	for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += theSample){ 
		for(xMesh = 0, xObserver = theObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += theSample, ++i){ 
			
			aDistanceLeft	= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_LEFT);
			aDistanceRight	= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_RIGHT);
			aDistanceUp		= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_UP);
			aDistanceDown	= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_DOWN);

			aTotalDistance	= aDistanceRight + aDistanceLeft + aDistanceUp + aDistanceDown;
			aBeachHeight	= floor((aTotalDistance/CK_COAST_MAX_SEA_DISTANCE) * getMaxBeachHeight());

			// If we are far away from the coast, we use the height information of the land portion.
			// If we are close to the coast, we use the beach height, so we can produce a smooth transition
			// between land and water.
			//aHeight = (unsigned char)(aTotalDistance < CK_COAST_MAX_SEA_DISTANCE ? aBeachHeight : theHeightData[i]);
			if(theHeightData[i] > 5) {
				printf("%u, ", theHeightData[i]);
			}
			aHeight = theHeightData[i];
			
			// Avoid negative heights... 
			aHeight = aHeight < 0 ? 0 : aHeight;

			theHeightData[i] = theMapGenerator->isLand(xObserver, zObserver) ? aHeight : (unsigned char)CK_SEA_BOTTON;
		}
	}
}

int CharackCoastGenerator::distanceFromWater(CharackMapGenerator *theMapGenerator, float theXObserver, float theZObserver, int theSample, int theDirection) {
	int aDim = CK_DIM_TERRAIN, aDistance = 0, aSteps = 0;
	
	theSample  *= theDirection;

	while(aSteps < getMaxSteps()) {
		if(!theMapGenerator->isLand(theXObserver, theZObserver)) {
			break; // water have been found
		} else {
			if(theDirection == MOVE_RIGHT || theDirection == MOVE_LEFT) {
				theXObserver += theSample;
			} else {
				theZObserver += theSample;
			}
			aDistance++;
			aSteps++;
		}
	}

	return aDistance;
}