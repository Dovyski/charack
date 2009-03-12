#include "CharackCoastGenerator.h"

CharackCoastGenerator::CharackCoastGenerator() {
	mMaxDistance = 5;
}

CharackCoastGenerator::~CharackCoastGenerator() {
}

void CharackCoastGenerator::setMaxDistance(int theDistance) {
	mMaxDistance = theDistance <= 0 ? 0 : theDistance;
}

int CharackCoastGenerator::getMaxDistance() {
	return mMaxDistance;
}

void CharackCoastGenerator::setRandSeed(int theSeed) {
	srand(theSeed);
}


void CharackCoastGenerator::disturbStraightCoastLines(unsigned char *theHeightData,unsigned char *theLandAndWaterData) {
	int aDim = DIM_TERRAIN + 1, i, aDistanceLeft, aDistanceRight, aDistanceUp, aDistanceDown;
	unsigned char aHeight;

	for(i = 0; i < aDim * aDim; i++){
		aDistanceLeft	= distanceFromWater(theLandAndWaterData, i, MOVE_LEFT);
		aDistanceRight	= distanceFromWater(theLandAndWaterData, i, MOVE_RIGHT);
		aDistanceUp		= distanceFromWater(theLandAndWaterData, i, MOVE_UP);
		aDistanceDown	= distanceFromWater(theLandAndWaterData, i, MOVE_DOWN);

		aHeight = theHeightData[i]; //+ (aDistanceRight*25 /*+ aDistanceRight + aDistanceUp + aDistanceDown*/);		
		//aHeight = (aDistanceLeft + aDistanceRight + aDistanceUp + aDistanceDown) * 100;
		aHeight = aHeight < 0 ? 0 : aHeight;
		//printf("%u, ", aHeight);

		theHeightData[i] = theLandAndWaterData[i] ? aHeight : (unsigned char)CK_SEA_BOTTON;
	}
}

int CharackCoastGenerator::distanceFromWater(unsigned char *theLandAndWaterData, int theIndex, int theDirection) {
	int aDim = DIM_TERRAIN + 1, z, aJump = 0, aDistance = 0, aSteps = 0;

	z		= theDirection == MOVE_RIGHT || theDirection == MOVE_LEFT ? _CK_CG_CALCULATE_J(theIndex) : _CK_CG_CALCULATE_I(theIndex);
	aJump	= theDirection == MOVE_RIGHT || theDirection == MOVE_LEFT ? 1 : DIM_TERRAIN;
	aJump  *= theDirection;

	while(_CK_CG_CONSTRAINT_CHECK(z) && aSteps < CK_COAST_MAX_STEP) {		
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