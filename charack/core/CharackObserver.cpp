/**
 * Copyright (c) 2009, Fernando Bevilacqua
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Universidade Federal de Santa Maria nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Fernando Bevilacqua ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Fernando Bevilacqua BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CharackObserver.h"
#include <stdio.h>

CharackObserver::CharackObserver() {
	mRotX = 0;
	mRotY = 0;
	mPosition = new CharackVector3(1169740, 400, -40); //TODO: fix this
}

CharackObserver::~CharackObserver() {
}

void CharackObserver::moveForward(int theHowMuch) {
	mPosition->z -= theHowMuch;
	nomalizePosition();
}

void CharackObserver::moveBackward(int theHowMuch){
	mPosition->z += theHowMuch;
	nomalizePosition();
}

void CharackObserver::moveLeft(int theHowMuch){
	mPosition->x -= theHowMuch;
	nomalizePosition();
}

void CharackObserver::moveRight(int theHowMuch){
	mPosition->x += theHowMuch;
	nomalizePosition();
}

void CharackObserver::moveUpDown(int theHowMuch) {
	mPosition->y += theHowMuch;
}

void CharackObserver::nomalizePosition(void) {
	// TODO: fix?
	return;
	mPosition->x = mPosition->x < 0 ? 0 : mPosition->x;
	mPosition->z = mPosition->z < 0 ? 0 : mPosition->z;

	mPosition->x = mPosition->x > CK_MAX_WIDTH ? CK_MAX_WIDTH : mPosition->x;
	mPosition->z = mPosition->z < CK_MAX_WIDTH ? CK_MAX_WIDTH : mPosition->z;
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


CharackVector3 *CharackObserver::getPosition() {
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