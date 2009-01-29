#include "CharackWorldSlice.h"

CharackWorldSlice::CharackWorldSlice(CharackWorld *theWorld) {
	mWorld			= theWorld;
	mOldObserverPos = Vector3(CK_MAX_WIDTH, 0, CK_MAX_WIDTH);
	mOldSample		= -1;
	mData			= (unsigned char *)malloc((DIM_TERRAIN + 1) * (DIM_TERRAIN + 1));
}

CharackWorldSlice::~CharackWorldSlice() {
}

unsigned char *CharackWorldSlice::getData() {
	return mData;
}

int CharackWorldSlice::updateData() {
	CharackObserver *aObserver =  mWorld->getObserver();
	int aSample = mWorld->getSample(), aReturn = 0;	
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();
	
	// If the observer has changed its location since last update, then we recreate the terrain again, otherwise
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
			recreateAllData();

		} else if (zObserver > mOldObserverPos.z) {
			shiftData(CharackWorldSlice::MOVE_BACKWARD);

		} else if (zObserver < mOldObserverPos.z) {
			shiftData(CharackWorldSlice::MOVE_FORWARD);

		}
		
		mOldObserverPos.x = aObserver->getPositionX();
		mOldObserverPos.z = aObserver->getPositionZ();
		mOldSample		  = aSample;
	}

	return aReturn;
}

void CharackWorldSlice::shiftData(int theDirection) {
	CharackObserver *aObserver =  mWorld->getObserver();
	
	int xMesh,zMesh, aDim = DIM_TERRAIN + 1, i = 0, aSample = mWorld->getSample(), aJump = abs(aObserver->getPositionX() - mOldObserverPos.x);
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();

	switch(theDirection) {
		case CharackWorldSlice::MOVE_RIGHT:
			for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
				for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample, i++){ 
					if((i + aJump < aDim * aDim) && (xMesh + aJump < aDim)) {
						mData[i] = mData[i + aJump];
					} else {
						mData[i] = (char)mWorld->getHeight(xObserver, zObserver);
					}
				}
			}
			
			printf("CharackWorldSlice::shiftData: RIGHT\n");
			break;

		case CharackWorldSlice::MOVE_LEFT:
			printf("CharackWorldSlice::shiftData: LEFT\n");
			break;

		case CharackWorldSlice::MOVE_FORWARD:
			printf("CharackWorldSlice::shiftData: FORWARD\n");
			break;

		case CharackWorldSlice::MOVE_BACKWARD:
			printf("CharackWorldSlice::shiftData: BACKWARD\n");
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
			mData[i++] = (char)mWorld->getHeight(xObserver, zObserver);
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
				fprintf(aFile, "mData[%d] = %d  [zMesh = %d, xMesh = %d] (xObserver = %.2f, zObserver = %.2f)\n", i, mData[i++], zMesh, xMesh, xObserver, zObserver);
			}
			fprintf(aFile, "--------\n");
		}
		
		fclose(aFile);
	} else {
		printf("CharackWorldSlice::dumpToFile - Could not open dump file [%s]\n", thePath);
	}
}