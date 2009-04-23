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

#include "CharackCoastGenerator.h"

CharackCoastGenerator::CharackCoastGenerator() {
	mMaxSteps		= CK_COAST_MAX_STEP;
	mMaxBeachHeight = CK_COAST_BEACH_HEIGHT;
}

CharackCoastGenerator::~CharackCoastGenerator() {
}

void CharackCoastGenerator::setMaxSteps(int theValue) {
	mMaxSteps = theValue <= 0 ? 0 : theValue;
}

int CharackCoastGenerator::getMaxSteps() {
	return mMaxSteps;
}

void CharackCoastGenerator::setMaxBeachHeight(float theValue) {
	mMaxBeachHeight = theValue <= 0 ? 0 : theValue;
}

float CharackCoastGenerator::getMaxBeachHeight() {
	return mMaxBeachHeight;
}

void CharackCoastGenerator::disturbStraightCoastLines(float *theHeightData, CharackMapGenerator *theMapGenerator, CharackObserver *theObserver, int theSample) {
	int aDim = CK_DIM_TERRAIN, i = 0, aDistanceLeft, aDistanceRight, aDistanceUp, aDistanceDown, xMesh,zMesh;
	float aHeight = 0;
	float xObserver = theObserver->getPositionX(), zObserver = theObserver->getPositionZ(), aBeachHeight = 0, aTotalDistance = 0;

	for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += theSample){ 
		for(xMesh = 0, xObserver = theObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += theSample, ++i){ 
			// We must analyze "land pixels" and "water pixels" differently. For all the "land pixels", we must check the distance between
			// them and the water and, according to this, apply the right erosion. It will produce beachs.
			// For "water pixels", we check the distance between them and the coast line and, using that information, we disturb the pixels
			// around to produce a disturbed coast line. It will produce more realistic coast lines.

			if(theMapGenerator->isLand(xObserver, zObserver)) {
				// We have found a land pixel...

				aDistanceLeft	= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_LEFT);
				aDistanceRight	= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_RIGHT);
				aDistanceUp		= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_UP);
				aDistanceDown	= distanceFromWater(theMapGenerator, xObserver, zObserver, theSample, MOVE_DOWN);

				aTotalDistance	= (float)(aDistanceRight + aDistanceLeft + aDistanceUp + aDistanceDown);
				aBeachHeight	= floor((aTotalDistance/CK_COAST_MAX_SEA_DISTANCE) * getMaxBeachHeight());

				// If we are far away from the coast, we use the height information of the land portion.
				// If we are close to the coast, we use the beach height, so we can produce a smooth transition
				// between land and water.
				aHeight = aTotalDistance < CK_COAST_MAX_SEA_DISTANCE ? aBeachHeight : theHeightData[i];
		
				// Avoid negative heights... 
				aHeight = aHeight < 0 ? 0 : aHeight;

				theHeightData[i] = aHeight;
			} else {
				// We have found a water pixels...
				theHeightData[i] = CK_SEA_BOTTON;
			}
		}
	}
}

int CharackCoastGenerator::distanceFromWater(CharackMapGenerator *theMapGenerator, float theXObserver, float theZObserver, int theSample, int theDirection) {
	int aDim = CK_DIM_TERRAIN, aDistance = 0, aSteps = 0;
	
	theSample  *= theDirection;

	while(aSteps < getMaxSteps()) {
		if(!theMapGenerator->isLand(theXObserver, theZObserver)) {
			break; // water have been found
		} else {
			if(theDirection == MOVE_RIGHT || theDirection == MOVE_LEFT) {
				theXObserver += theSample;
			} else {
				theZObserver += theSample;
			}
			aDistance++;
			aSteps++;
		}
	}

	return aDistance;
}