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
	mPerlinNoise		= new Perlin(16, 8, 1, 10);
	mMaxStepsLand		= CK_COAST_MAX_STEP;
	mMaxStepsWater		= CK_COAST_DISTRB_MAX_STEP;
	mMaxBeachHeight		= CK_COAST_BEACH_HEIGHT;
}

CharackCoastGenerator::~CharackCoastGenerator() {
}

void CharackCoastGenerator::setMaxStepsLand(int theValue) {
	mMaxStepsLand = theValue <= 0 ? 0 : theValue;
}

int CharackCoastGenerator::getMaxStepsLand(int theTileType) {
	// TODO: set maxStep dynamically?
	return theTileType == CharackMapGenerator::LAND_COAST ? mMaxStepsLand : mMaxStepsLand*2;
}

void CharackCoastGenerator::setMaxStepsWater(int theValue) {
	mMaxStepsLand = theValue <= 0 ? 0 : theValue;
}

int CharackCoastGenerator::getMaxStepsWater() {
	return mMaxStepsWater;
}

void CharackCoastGenerator::setMaxBeachHeight(float theValue) {
	mMaxBeachHeight = theValue <= 0 ? 0 : theValue;
}

float CharackCoastGenerator::getMaxBeachHeight() {
	return mMaxBeachHeight;
}

void CharackCoastGenerator::disturbStraightCoastLines(float *theHeightData, CharackMapGenerator *theMapGenerator, CharackObserver *theObserver, int theSample) {
	float	xObserver		= theObserver->getPositionX(),
			zObserver		= theObserver->getPositionZ(),
			aTotalDistance	= 0;

	int		aDim			= CK_DIM_TERRAIN,
			i				= 0,
			aDistanceLeft	= 0,
			aDistanceRight	= 0,
			aDistanceUp		= 0,
			aDistanceDown	= 0,
			xMesh			= 0,
			zMesh			= 0,
			aTileType		= theMapGenerator->getDescription(xObserver, zObserver),
			aMaxStepLand	= getMaxStepsLand(aTileType);

	for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += theSample){ 
		for(xMesh = 0, xObserver = theObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += theSample, ++i){ 
			// We must analyze "land pixels" and "water pixels" differently. For all the "land pixels", we must check the distance between
			// them and the water and, according to this, apply the right erosion. It will produce beachs.
			// For "water pixels", we check the distance between them and the coast line and, using that information, we disturb the pixels
			// around to produce a disturbed coast line. It will produce more realistic coast lines.

			if(theMapGenerator->isLand(xObserver, zObserver)) {
				// We have found a land pixel...

				aDistanceLeft	= theMapGenerator->distanceFrom(CharackMapGenerator::WATER, CharackMapGenerator::RESOLUTION_HIGH, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_LEFT,	 aMaxStepLand);
				aDistanceRight	= theMapGenerator->distanceFrom(CharackMapGenerator::WATER, CharackMapGenerator::RESOLUTION_HIGH, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_RIGHT,	 aMaxStepLand);
				aDistanceUp		= theMapGenerator->distanceFrom(CharackMapGenerator::WATER, CharackMapGenerator::RESOLUTION_HIGH, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_UP,	 aMaxStepLand);
				aDistanceDown	= theMapGenerator->distanceFrom(CharackMapGenerator::WATER, CharackMapGenerator::RESOLUTION_HIGH, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_DOWN,	 aMaxStepLand);

				aTotalDistance	= (float)(aDistanceRight + aDistanceLeft + aDistanceUp + aDistanceDown);

				// If we are far away from the coast, we use the height information of the land portion.
				// If we are close to the coast, we use the beach height, so we can produce a smooth transition
				// between land and water.
				theHeightData[i] = aTotalDistance < aMaxStepLand * 4 ? generateBeachHeight(aTotalDistance, aMaxStepLand) : theHeightData[i];
			} else {
				// We have found a water pixels...
				if(CK_COAST_DISTURBE) {
					aDistanceLeft	= theMapGenerator->distanceFrom(CharackMapGenerator::LAND, CharackMapGenerator::RESOLUTION_LOW, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_LEFT,	getMaxStepsWater());
					aDistanceRight	= theMapGenerator->distanceFrom(CharackMapGenerator::LAND, CharackMapGenerator::RESOLUTION_LOW, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_RIGHT,	getMaxStepsWater());
					aDistanceUp		= theMapGenerator->distanceFrom(CharackMapGenerator::LAND, CharackMapGenerator::RESOLUTION_LOW, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_UP,		getMaxStepsWater());
					aDistanceDown	= theMapGenerator->distanceFrom(CharackMapGenerator::LAND, CharackMapGenerator::RESOLUTION_LOW, xObserver, zObserver, theSample, CharackMapGenerator::MOVE_DOWN,	getMaxStepsWater());

					aTotalDistance	 = (float)(aDistanceRight + aDistanceLeft + aDistanceUp + aDistanceDown);
					theHeightData[i] = generateDisturbedCoastLineHeight(aTotalDistance, xObserver, zObserver);
				} else {
					theHeightData[i] = CK_SEA_BOTTON;
				}
			}
		}
	}
}


float CharackCoastGenerator::generateBeachHeight(float theTotalDistance, int theMaxStepLand) {
	float aRet = floor((theTotalDistance/(theMaxStepLand * 4)) * getMaxBeachHeight());
	return aRet < 0 ? 0 : aRet; 
}

float CharackCoastGenerator::generateDisturbedCoastLineHeight(float theTotalDistance, float theXObserver, float theZObserver) {
	float aRet = CK_SEA_BOTTON;
	
	if(theTotalDistance < CK_COAST_DISTRB_MAX_DISTANCE) {
		aRet = abs(mPerlinNoise->Get(theXObserver/10, theZObserver/10));
		aRet = aRet > CK_COAST_DISTURBE_RATE ? aRet * getMaxBeachHeight() : CK_SEA_BOTTON;
	}

	return aRet;
}