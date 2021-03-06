#include "CharackObserver.h"
#include <stdio.h>

CharackObserver::CharackObserver() {
	mRotX = 0;
	mRotY = 0;
	mPosition = new Vector3(0, -240, 0); //TODO: fix this
}

CharackObserver::~CharackObserver() {
}

void CharackObserver::moveForward(int theHowMuch) {
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(360 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(360 - mRotY));

	nomalizePosition();
}

void CharackObserver::moveBackward(int theHowMuch){
	mPosition->z = mPosition->z - theHowMuch * cos(CK_DEG2RAD(360 - mRotY));
	mPosition->x = mPosition->x - theHowMuch * sin(CK_DEG2RAD(360 - mRotY));

	nomalizePosition();
}

void CharackObserver::moveLeft(int theHowMuch){
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(90 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(90 - mRotY));

	nomalizePosition();
}

void CharackObserver::moveRight(int theHowMuch){
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(270 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(270 - mRotY));

	nomalizePosition();
}

void CharackObserver::moveUpDown(int theHowMuch) {
	mPosition->y += theHowMuch;
}

void CharackObserver::nomalizePosition(void) {
	mPosition->x = mPosition->x > 0 ? 0 : mPosition->x;
	mPosition->z = mPosition->z > 0 ? 0 : mPosition->z;

	mPosition->x = abs(mPosition->x) >= CK_MAX_WIDTH ? -CK_MAX_WIDTH : mPosition->x;
	mPosition->z = abs(mPosition->z) >= CK_MAX_WIDTH ? -CK_MAX_WIDTH : mPosition->z;
}

/**
 * Negative = rotate to left, positive rotate to right
 */
void CharackObserver::rotateLookLeftRight(int theDegres) {
	mRotY += theDegres;
}

/**
 * Positive = look up, negative = look down
 */
void CharackObserver::rotateLookUpDown(int theDegres){
	mRotX += theDegres;
}

float CharackObserver::getPositionX() {
	return mPosition->x;
}

float CharackObserver::getPositionY() {
	return mPosition->y;
}

float CharackObserver::getPositionZ() {
	return mPosition->z;
}


void CharackObserver::setPosition(float theX, float theY, float theZ) {
	mPosition->x = theX;
	mPosition->y = theY;
	mPosition->z = theZ;

	nomalizePosition();
}


Vector3 *CharackObserver::getPosition() {
	return mPosition;
}

int CharackObserver::getRotationX() {
	return mRotX;
}

int CharackObserver::getRotationY() {
	return mRotY;
}

void CharackObserver::setRotationX(int theRotation) {
	mRotX = theRotation;
}

void CharackObserver::setRotationY(int theRotation) {
	mRotY = theRotation;
}