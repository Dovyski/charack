#include "CharackCamera.h"
#include <stdio.h>

CharackCamera::CharackCamera() {
	mRotX = 0;
	mRotY = 0;
	mPosition = new Vector3(-1000, 100, -1000); //TODO: fix this
}

CharackCamera::~CharackCamera() {
}

void CharackCamera::moveForward(int theHowMuch) {
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(360 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(360 - mRotY));
}

void CharackCamera::moveBackward(int theHowMuch){
	mPosition->z = mPosition->z - theHowMuch * cos(CK_DEG2RAD(360 - mRotY));
	mPosition->x = mPosition->x - theHowMuch * sin(CK_DEG2RAD(360 - mRotY));
}

void CharackCamera::moveLeft(int theHowMuch){
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(90 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(90 - mRotY));
}

void CharackCamera::moveRight(int theHowMuch){
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(270 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(270 - mRotY));
}

void CharackCamera::moveUpDown(int theHowMuch) {
	mPosition->y += theHowMuch;
}

/**
 * Negative = rotate to left, positive rotate to right
 */
void CharackCamera::rotateLookLeftRight(int theDegres) {
	mRotY += theDegres;
}

/**
 * Positive = look up, negative = look down
 */
void CharackCamera::rotateLookUpDown(int theDegres){
	mRotX += theDegres;
}

float CharackCamera::getPositionX() {
	return mPosition->x;
}

float CharackCamera::getPositionY() {
	return mPosition->y;
}

float CharackCamera::getPositionZ() {
	return mPosition->z;
}

Vector3 *CharackCamera::getPosition() {
	return mPosition;
}

int CharackCamera::getRotationX() {
	return mRotX;
}

int CharackCamera::getRotationY() {
	return mRotY;
}