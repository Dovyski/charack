#include "CharackObserver.h"
#include <stdio.h>

CharackObserver::CharackObserver() {
	mRotX = 0;
	mRotY = 0;
	mPosition = new Vector3(-55000, 60, -55000); //TODO: fix this
}

CharackObserver::~CharackObserver() {
}

void CharackObserver::moveForward(int theHowMuch) {
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(360 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(360 - mRotY));
}

void CharackObserver::moveBackward(int theHowMuch){
	mPosition->z = mPosition->z - theHowMuch * cos(CK_DEG2RAD(360 - mRotY));
	mPosition->x = mPosition->x - theHowMuch * sin(CK_DEG2RAD(360 - mRotY));
}

void CharackObserver::moveLeft(int theHowMuch){
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(90 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(90 - mRotY));
}

void CharackObserver::moveRight(int theHowMuch){
	mPosition->z = mPosition->z + theHowMuch * cos(CK_DEG2RAD(270 - mRotY));
	mPosition->x = mPosition->x + theHowMuch * sin(CK_DEG2RAD(270 - mRotY));
}

void CharackObserver::moveUpDown(int theHowMuch) {
	mPosition->y += theHowMuch;
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