#include "CharackWorldSlice.h"

CharackWorldSlice::CharackWorldSlice(CharackWorld *theWorld) {
	mWorld			= theWorld;
	mOldObserverPos = Vector3(CK_MAX_WIDTH, 0, CK_MAX_WIDTH);
	mOldSample		= -1;
	mHeightData		= (unsigned char *)malloc((DIM_TERRAIN + 1) * (DIM_TERRAIN + 1));
	mLandWaterData	= (unsigned char *)malloc((DIM_TERRAIN + 1) * (DIM_TERRAIN + 1));
}

CharackWorldSlice::~CharackWorldSlice() {
}

unsigned char *CharackWorldSlice::getHeightData() {
	return mHeightData;
}

unsigned char *CharackWorldSlice::getLandAndWaterData() {
	return mLandWaterData;
}


void CharackWorldSlice::generateLandAndWaterData() {
	int xMesh,zMesh, aDim = DIM_TERRAIN + 1, i = 0, aSample = mWorld->getSample();
	
	CharackObserver *aObserver =  mWorld->getObserver();
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
		for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample){ 
			mLandWaterData[i++] = mWorld->getMapGenerator()->isLand(xObserver, zObserver);
		}
	}

	printf("CharackWorldSlice::generateLandAndWaterData()\n");
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
			shiftData(CharackWorldSlice::MOVE_RIGHT);

		} else if (xObserver < mOldObserverPos.x) {
			shiftData(CharackWorldSlice::MOVE_LEFT);

		} else if (zObserver > mOldObserverPos.z) {
			shiftData(CharackWorldSlice::MOVE_BACKWARD);

		} else if (zObserver < mOldObserverPos.z) {
			shiftData(CharackWorldSlice::MOVE_FORWARD);
		}
		
		mOldObserverPos.x = aObserver->getPositionX();
		mOldObserverPos.z = aObserver->getPositionZ();
		mOldSample		  = aSample;

		generateLandAndWaterData();
	}

	return aReturn;
}

void CharackWorldSlice::shiftData(int theDirection) {
	CharackObserver *aObserver =  mWorld->getObserver();
	
	int xMesh,zMesh, aDim = DIM_TERRAIN + 1, i = 0, aSample = mWorld->getSample(), aJump = 0;
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	switch(theDirection) {
		case CharackWorldSlice::MOVE_RIGHT:
			aJump = abs(aObserver->getPositionX() - mOldObserverPos.x);

			for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
				for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample, i++){ 
					if((i + aJump < aDim * aDim) && (xMesh + aJump < aDim)) {
						mHeightData[i] = mHeightData[i + aJump];
					} else {
						mHeightData[i] = (char)mWorld->getHeight(xObserver, zObserver);
					}
				}
			}
			
			printf("CharackWorldSlice::shiftData: RIGHT\n");
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
						mHeightData[i] = (char)mWorld->getHeight(xObserver, zObserver);
					}
				}
			}

			printf("CharackWorldSlice::shiftData: LEFT\n");
			break;

		case CharackWorldSlice::MOVE_FORWARD:
			printf("CharackWorldSlice::shiftData: FORWARD\n");
			recreateAllData();
			break;

		case CharackWorldSlice::MOVE_BACKWARD:
			printf("CharackWorldSlice::shiftData: BACKWARD\n");
			recreateAllData();
			break;

		default:
			printf("CharackWorldSlice::shiftData: RIGHT (default)\n");
	}
}

void CharackWorldSlice::recreateAllData() {
	int xMesh,zMesh, aDim = DIM_TERRAIN + 1, i = 0, aSample = mWorld->getSample();
	
	CharackObserver *aObserver =  mWorld->getObserver();
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
		for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample){ 
			mHeightData[i++] = (char)mWorld->getHeight(xObserver, zObserver);
		}
	}

	printf("CharackWorldSlice::recreateAllData()\n");
}


void CharackWorldSlice::dumpToFile(char *thePath) {
	FILE *aFile;
	int xMesh,zMesh, aDim = DIM_TERRAIN + 1, i = 0, aSample = mWorld->getSample();
	
	CharackObserver *aObserver =  mWorld->getObserver();
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	aFile = fopen(thePath, "w+");

	if(aFile != NULL) {
		for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
			for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample){ 
				fprintf(aFile, "mHeightData[%d] = %d  [zMesh = %d, xMesh = %d] (xObserver = %.2f, zObserver = %.2f)\n", i, mHeightData[i++], zMesh, xMesh, xObserver, zObserver);
			}
			fprintf(aFile, "--------\n");
		}
		
		fclose(aFile);
	} else {
		printf("CharackWorldSlice::dumpToFile - Could not open dump file [%s]\n", thePath);
	}
}