#include "CharackLineSegment.h"

CharackLineSegment::CharackLineSegment(Vector3 thePointA, Vector3 thePointB, int theOrientationAxis) {
	mPointA = thePointA;
	mPointB = thePointB;
	mOrientationAxis = theOrientationAxis; 
}

CharackLineSegment::~CharackLineSegment() {
}

Vector3 CharackLineSegment::getPointA() {
	return mPointA;
}

Vector3 CharackLineSegment::getPointB() {
	return mPointB;
}

int CharackLineSegment::getOrientationAxis() {
	return mOrientationAxis;
}