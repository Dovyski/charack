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

#include "CharackWorldSlice.h"

CharackWorldSlice::CharackWorldSlice(CharackWorld *theWorld) {
	mWorld			= theWorld;
	mOldObserverPos = CharackVector3(CK_MAX_WIDTH, 0, CK_MAX_WIDTH);
	mOldSample		= -1;
	mHeightData		= (float *)malloc(CK_DIM_TERRAIN * CK_DIM_TERRAIN * sizeof(float));
	mLandWaterData	= (float *)malloc(CK_DIM_TERRAIN * CK_DIM_TERRAIN * sizeof(float));
}

CharackWorldSlice::~CharackWorldSlice() {
	free(mHeightData);
	mHeightData = NULL;
	
	free(mLandWaterData);
	mLandWaterData = NULL;
}

float *CharackWorldSlice::getHeightData() {
	return mHeightData;
}

float *CharackWorldSlice::getLandAndWaterData() {
	return mLandWaterData;
}


void CharackWorldSlice::generateLandAndWaterData() {
	int xMesh,zMesh, aDim = CK_DIM_TERRAIN, i = 0, aSample = mWorld->getSample();
	
	CharackObserver *aObserver =  mWorld->getObserver();
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
		for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample){ 
			mLandWaterData[i++] = mWorld->getMapGenerator()->isLand(xObserver, zObserver);
		}
	}
}

int CharackWorldSlice::updateData() {
	CharackObserver *aObserver =  mWorld->getObserver();
	int aSample = mWorld->getSample(), aReturn = 0;	
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();
	
	// If the observer has changed his location since last update, then we recreate the terrain again, otherwise
	// we can use the old data (which has no modifications).

	if(mOldObserverPos.x != xObserver || mOldObserverPos.z != zObserver || mOldSample != aSample) {
		// We have changes, time to update the mesh
		aReturn = 1;

		if(mOldSample != aSample) {
			recreateAllData();

		} else if(xObserver > mOldObserverPos.x) {
			recreateAllData();

		} else if (xObserver < mOldObserverPos.x) {
			recreateAllData();

		} else if (zObserver > mOldObserverPos.z) {
			shiftData(CharackWorldSlice::MOVE_BACKWARD);

		} else if (zObserver < mOldObserverPos.z) {
			shiftData(CharackWorldSlice::MOVE_FORWARD);
		}
		
		mOldObserverPos.x = aObserver->getPositionX();
		mOldObserverPos.z = aObserver->getPositionZ();
		mOldSample		  = aSample;

		// TODO: fix!
		//generateLandAndWaterData();
	}

	return aReturn;
}

void CharackWorldSlice::shiftData(int theDirection) {
	CharackObserver *aObserver =  mWorld->getObserver();
	
	int xMesh,zMesh, aDim = CK_DIM_TERRAIN, i = 0, aSample = mWorld->getSample(), aJump = 0;
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	switch(theDirection) {
		case CharackWorldSlice::MOVE_RIGHT:
			aJump = abs(aObserver->getPositionX() - mOldObserverPos.x);

			for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
				for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample, i++){ 
					if((i + aJump < aDim * aDim) && (xMesh + aJump < aDim)) {
						mHeightData[i] = mHeightData[i + aJump];
					} else {
						mHeightData[i] = mWorld->getHeight(xObserver, zObserver);
					}
				}
			}
			
			//printf("CharackWorldSlice::shiftData: RIGHT\n");
			break;

		case CharackWorldSlice::MOVE_LEFT:
			aJump = abs(aObserver->getPositionX() - mOldObserverPos.x);
			i = aDim * aDim - 1;
			zObserver = zObserver + aDim * aSample;

			for(zMesh = aDim - 1; zMesh >= 0; zMesh--, zObserver -= aSample){ 
				for(xMesh = aDim - 1, xObserver = aObserver->getPositionX() + aDim * aSample; xMesh >= 0; xMesh--, xObserver -= aSample, i--){ 
					if((i - aJump >= 0) && (xMesh >= aJump)) {
						mHeightData[i] = mHeightData[i - aJump];
					} else if(i >= 0){
						mHeightData[i] = mWorld->getHeight(xObserver, zObserver);
					}
				}
			}

			//printf("CharackWorldSlice::shiftData: LEFT\n");
			break;

		case CharackWorldSlice::MOVE_FORWARD:
			//printf("CharackWorldSlice::shiftData: FORWARD\n");
			recreateAllData();
			break;

		case CharackWorldSlice::MOVE_BACKWARD:
			//printf("CharackWorldSlice::shiftData: BACKWARD\n");
			recreateAllData();
			break;

		default:
			printf("CharackWorldSlice::shiftData: RIGHT (default)\n");
	}
}

void CharackWorldSlice::recreateAllData() {
	int xMesh,zMesh, aDim = CK_DIM_TERRAIN, i = 0, aSample = mWorld->getSample();
	
	CharackObserver *aObserver =  mWorld->getObserver();
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	mWorld->getBenchmark()->startClock();

	for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
		for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample){ 
			mHeightData[i++] = mWorld->getHeight(xObserver, zObserver);
		}
	}

	mWorld->getBenchmark()->stopClock(CharackBenchmark::LABEL_RAW_HEIGHTMAP_GENERATION);

	//printf("CharackWorldSlice::recreateAllData()\n");
}


void CharackWorldSlice::dumpToFile(char *thePath) {
	FILE *aFile;
	int aDim = CK_DIM_TERRAIN;

	aFile = fopen(thePath, "w+");

	if(aFile != NULL) {
		fwrite(mHeightData, sizeof(float), aDim*aDim, aFile);
		fclose(aFile);
	} else {
		printf("CharackWorldSlice::dumpToFile - Could not open dump file [%s]\n", thePath);
	}
}