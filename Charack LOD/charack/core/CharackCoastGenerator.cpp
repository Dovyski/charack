#include "CharackCoastGenerator.h"

bool __comparePointsX(Vector3 theFirst, Vector3 theSecond) {
	return theFirst.x < theSecond.x;
}

bool __comparePointsY(Vector3 theFirst, Vector3 theSecond) {
	return theFirst.y < theSecond.y;
}

CharackCoastGenerator::CharackCoastGenerator() {
	mMaxDivision	= 4;
	mMaxVariation	= 10;
}

CharackCoastGenerator::~CharackCoastGenerator() {
}

void CharackCoastGenerator::setMaxDivisions(int theHowMany) {
	mMaxDivision = theHowMany <= 0 ? 0 : theHowMany;
}

int CharackCoastGenerator::getMaxDivisions() {
	return mMaxDivision;
}

void CharackCoastGenerator::setVariation(int theHowMuch) {
	mMaxVariation = theHowMuch;
}

int CharackCoastGenerator::getVariation() {
	return mMaxVariation;
}

void CharackCoastGenerator::setRandSeed(int theSeed) {
	srand(theSeed);
}

std::list<Vector3> CharackCoastGenerator::generate(Vector3 thePointA, Vector3 thePointB, int thePerturbationAxis) {
	std::list<Vector3> aResult;	
	std::list<Vector3> aSegments;

	aSegments.push_back(thePointA);
	aSegments.push_back(thePointB);
	aSegments.push_back(Vector3((float)mMaxVariation, (float)mMaxDivision, 0));

	aResult.push_back(thePointA);
	
	while(aSegments.size() > 0) {
		Vector3 aPointA = aSegments.front();
		aSegments.pop_front();

		Vector3 aPointB = aSegments.front();
		aSegments.pop_front();

		Vector3 aSettings = aSegments.front();
		aSegments.pop_front();

		Vector3 aMidPoint = (aPointA + aPointB)/2.0;

		if(thePerturbationAxis == CharackCoastGenerator::AXIS_X) {
			aMidPoint.x = aMidPoint.x - _CK_CG_RRAND(-aSettings.x, aSettings.x);

		} else if(thePerturbationAxis == CharackCoastGenerator::AXIS_Y) {
			aMidPoint.y = aMidPoint.y - _CK_CG_RRAND(-aSettings.x, aSettings.x);
		}

		aSettings.x = aSettings.x/2;
		aSettings.y = aSettings.y - 1;

		if(aSettings.y > 0) {
			aSegments.push_back(aPointA);
			aSegments.push_back(aMidPoint);
			aSegments.push_back(aSettings);

			aSegments.push_back(aMidPoint);
			aSegments.push_back(aPointB);
			aSegments.push_back(aSettings);
		}

		aResult.push_back(aMidPoint);		
	}

	aResult.push_back(thePointB);
	
	if(thePerturbationAxis == CharackCoastGenerator::AXIS_X) {
		aResult.sort(__comparePointsY);

	} else if(thePerturbationAxis == CharackCoastGenerator::AXIS_Y) {
		aResult.sort(__comparePointsX);
	}

	return aResult;
}