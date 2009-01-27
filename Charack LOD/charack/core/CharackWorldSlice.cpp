#include "CharackWorldSlice.h"

CharackWorldSlice::CharackWorldSlice(CharackWorld *theWorld) {
	mWorld			= theWorld;
	mOldObserverPos = Vector3(CK_MAX_WIDTH, 0, CK_MAX_WIDTH);
	mOldSample		= mWorld->getSample();
	mData			= (unsigned char *)malloc((DIM_TERRAIN + 1) * (DIM_TERRAIN + 1));
}

CharackWorldSlice::~CharackWorldSlice() {
}

unsigned char *CharackWorldSlice::getData() {
	return mData;
}

int CharackWorldSlice::updateData() {
	int xMesh,zMesh, aDim = DIM_TERRAIN + 1, i = 0, aReturn = 0, aSample = mWorld->getSample();
	
	CharackObserver *aObserver =  mWorld->getObserver();
	float xObserver = aObserver->getPositionX(), zObserver = aObserver->getPositionZ();
	
	// If the observer has changed its location since last update, then we recreate the terrain again, otherwise
	// we can use the old data (which has no modifications).

	if(mOldObserverPos.x != xObserver || mOldObserverPos.z != zObserver || mOldSample != aSample) {
		// We have changes, time to update the mesh
		aReturn = 1;

		if(xObserver > mOldObserverPos.x) {
			// Moving to the right
			//printf("CharackWorldSlice::updateData: RIGHT\n");
		} else if (xObserver < mOldObserverPos.x) {
			// Moving to the left
			//printf("CharackWorldSlice::updateData: LEFT\n");
		} else if (zObserver > mOldObserverPos.z) {
			// Moving backward
			//printf("CharackWorldSlice::updateData: BACKWARD\n");
		} else if (zObserver < mOldObserverPos.z) {
			// Moving forward
			//printf("CharackWorldSlice::updateData: FORWARD\n");
		}

		for(zMesh = 0; zMesh < aDim; zMesh++, zObserver += aSample){ 
			for(xMesh = 0, xObserver = aObserver->getPositionX(); xMesh < aDim; xMesh++, xObserver += aSample){ 
				mData[i++] = (char)mWorld->getHeight(xObserver, zObserver);
			}
		}
		
		//printf("limpando cache old:(%.2f,%.2f), pos:(%.2f,%.2f)...\n", mOldObserverPos.x, mOldObserverPos.z, aObserver->getPositionX(), aObserver->getPositionZ());
		mOldObserverPos.x = aObserver->getPositionX();
		mOldObserverPos.z = aObserver->getPositionZ();
	}

	return aReturn;
}