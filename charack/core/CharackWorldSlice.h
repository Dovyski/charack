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

#ifndef __CHARACK_WORLDSLICE_H_
#define __CHARACK_WORLDSLICE_H_

#include "../util/CharackVector3.h"
#include "config.h"

class CharackWorld;

#include "CharackWorld.h"

#include <stdlib.h>
#include <stdio.h>

class CharackWorldSlice {
	private:
		float *mHeightData;
		float *mLandWaterData;
		CharackVector3 mOldObserverPos;
		int mOldSample;
		CharackWorld *mWorld;

		void generateLandAndWaterData();

	public:
		static const int MOVE_RIGHT		= 0;
		static const int MOVE_LEFT		= 1;
		static const int MOVE_FORWARD	= 2;
		static const int MOVE_BACKWARD	= 3;

		CharackWorldSlice(CharackWorld *theWorld);		
		~CharackWorldSlice();

		float *getHeightData();
		float *getLandAndWaterData();
		int updateData();
		void shiftData(int theDirection);
		void recreateAllData();
		void dumpToFile(char *thePath);
};

#endif