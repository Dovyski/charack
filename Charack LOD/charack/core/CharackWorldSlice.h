#ifndef __CHARACK_WORLDSLICE_H_
#define __CHARACK_WORLDSLICE_H_

#include "../tools/vector3.h"
#include "../defs.h"
#include "config.h"

class CharackWorld;

#include "CharackWorld.h"

#include <stdlib.h>
#include <stdio.h>

class CharackWorldSlice {
	private:
		unsigned char *mData;
		Vector3 mOldObserverPos;
		int mOldSample;
		CharackWorld *mWorld;

	public:
		CharackWorldSlice(CharackWorld *theWorld);		
		~CharackWorldSlice();

		unsigned char *getData();
		int updateData();
};

#endif