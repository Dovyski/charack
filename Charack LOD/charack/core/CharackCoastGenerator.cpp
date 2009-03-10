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
	int aDim = DIM_TERRAIN + 1, i, aIsLand;

	for(i = 0; i < aDim * aDim; i++){ 
		// theLandAndWaterData == 1 means land...
		theHeightData[i] = theLandAndWaterData[i] ? theHeightData[i] : 0;
	}

	//printf("CharackCoastGenerator::disturbStraightCoastLines()\n");
}