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
		static const int MOVE_RIGHT		= 0;
		static const int MOVE_LEFT		= 1;
		static const int MOVE_FORWARD	= 2;
		static const int MOVE_BACKWARD	= 3;

		CharackWorldSlice(CharackWorld *theWorld);		
		~CharackWorldSlice();

		unsigned char *getData();
		int updateData();
		void shiftData(int theDirection);
		void recreateAllData();
		void dumpToFile(char *thePath);
};

#endif