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

void CharackCoastGenerator::disturbStraightCoastLines(unsigned char *theHeightData,unsigned char *theLandAndWaterData) {
	int aDim = DIM_TERRAIN + 1, i, aDistanceLeft, aDistanceRight, aDistanceUp, aDistanceDown;
	unsigned char aHeight;
	float aBeachHeight = 0, aTotalDistance = 0;

	for(i = 0; i < aDim * aDim; i++){
		aDistanceLeft	= distanceFromWater(theLandAndWaterData, i, MOVE_LEFT);
		aDistanceRight	= distanceFromWater(theLandAndWaterData, i, MOVE_RIGHT);
		aDistanceUp		= distanceFromWater(theLandAndWaterData, i, MOVE_UP);
		aDistanceDown	= distanceFromWater(theLandAndWaterData, i, MOVE_DOWN);

		aTotalDistance	= aDistanceRight + aDistanceLeft + aDistanceUp + aDistanceDown;
		aBeachHeight	= floor((aTotalDistance/CK_COAST_MAX_SEA_DISTANCE) * getMaxBeachHeight());

		// If we are far away from the coast, we use the height information of the land portion.
		// If wr are close to the coast, we use the beach height, so we can produce a smooth transition
		// between land and water.
		aHeight = (unsigned char)(aTotalDistance < CK_COAST_MAX_SEA_DISTANCE ? aBeachHeight : theHeightData[i]);
		
		// Avoid negative heights... 
		aHeight = aHeight < 0 ? 0 : aHeight;

		theHeightData[i] = theLandAndWaterData[i] ? aHeight : (unsigned char)CK_SEA_BOTTON;
	}
}

int CharackCoastGenerator::distanceFromWater(unsigned char *theLandAndWaterData, int theIndex, int theDirection) {
	int aDim = DIM_TERRAIN + 1, z, aJump = 0, aDistance = 0, aSteps = 0;

	z		= theDirection == MOVE_RIGHT || theDirection == MOVE_LEFT ? _CK_CG_CALCULATE_J(theIndex) : _CK_CG_CALCULATE_I(theIndex);
	aJump	= theDirection == MOVE_RIGHT || theDirection == MOVE_LEFT ? 1 : DIM_TERRAIN;
	aJump  *= theDirection;

	while(_CK_CG_CONSTRAINT_CHECK(z) && aSteps < getMaxSteps()) {		
		if(theLandAndWaterData[theIndex] == 0) {
			break; // water have been found
		} else {
			z += theDirection;
			theIndex += aJump;
			aDistance++;
			aSteps++;
		}
	}

	return aDistance;
}