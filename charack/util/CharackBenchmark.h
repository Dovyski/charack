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

#ifndef __CHARACK_BENCHMARK_H_
#define __CHARACK_BENCHMARK_H_

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "../core/config.h"

#define CK_BENCH_MAX_LABELS				7
#define CK_BENCH_MAX_LEN_LABEL_NAME		60

class CharackBenchmark {
	private:
		long int mStartMark;
		long int mTimeCount[CK_BENCH_MAX_LABELS];
		char mLabelTexts[CK_BENCH_MAX_LABELS][CK_BENCH_MAX_LEN_LABEL_NAME];

		void init();

	public:
		static const char LABEL_MM_GENERATION				= 0;//
		static const char LABEL_RAW_HEIGHTMAP_GENERATION	= 1;//
		static const char LABEL_COASTLINE_GENERATION		= 2;//
		static const char LABEL_BEACH_GENERATION			= 3;//
		static const char LABEL_TERRAIN_SMOOTHING			= 4;
		static const char LABEL_RENDERIZATION				= 5;
		static const char LABEL_OCEAN_HEIGHTMAP				= 6;//

		CharackBenchmark();		
		~CharackBenchmark();

		void startClock();
		void stopClock(char theLabel);

		void printReport(char *thePath);
};

#endif