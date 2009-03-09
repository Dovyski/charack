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
	int aDim = DIM_TERRAIN + 1, i;

	for(i = 0; i < aDim * aDim; i++){ 
		//theHeightData[i] = theLandAndWaterData[i] ? theHeightData[i] : theHeightData[i] + 50;
		// TODO: the method
	}

	printf("CharackCoastGenerator::disturbStraightCoastLines()\n");
}