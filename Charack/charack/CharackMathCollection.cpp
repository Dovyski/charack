#include "CharackMathCollection.h"

CharackMathCollection::CharackMathCollection() {
	for(int i = 0; i < CK_MATHC_MAX_FUNCTION; i++) {
		mFunctions[i] = NULL;
	}
}

CharackMathCollection::~CharackMathCollection() {
}

int CharackMathCollection::getNextAvailablePos() {
	int i, aRet = -1;
	for(i = 0; i < CK_MATHC_MAX_FUNCTION; i++) {
		if(mFunctions[i] == NULL) {
			aRet = i;
			break;
		}
	}
	return aRet;
}


bool CharackMathCollection::addFunction(float (*theFunction)(float)) {
	int aPos = getNextAvailablePos();
	if(aPos >= 0) { 
		mFunctions[aPos] = theFunction;
	}
	return aPos > 0;
}

bool CharackMathCollection::removeFunction(float (*theFunction)(float)) {
	return true;
}

void CharackMathCollection::setWeights(float theWeights[]){
	for(int i = 0; i < CK_MATHC_MAX_FUNCTION; i++) {
		mWeights[i] = theWeights[i];
	}
}

float CharackMathCollection::getValue(float theThreshold) {
	float aResult = 0;
	
	for(int i = 0, j = 0; i < CK_MATHC_MAX_FUNCTION; i++) {
		if(mFunctions[i] != NULL) {
			aResult += mFunctions[i](theThreshold) * mWeights[j++];
		}
	}
	
	return aResult;
}