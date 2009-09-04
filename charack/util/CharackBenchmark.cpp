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

#include "CharackBenchmark.h"

CharackBenchmark::CharackBenchmark() {
	init();
}

CharackBenchmark::~CharackBenchmark() {
}

void CharackBenchmark::init() {
	for(int i = 0; i < CK_BENCH_MAX_LABELS; i++) {
		mTimeCount[i] = 0;
	}

	strcpy(mLabelTexts[CharackBenchmark::LABEL_MM_GENERATION],				"MM generation");
	strcpy(mLabelTexts[CharackBenchmark::LABEL_RAW_HEIGHTMAP_GENERATION],	"Raw heightmap generation");
	strcpy(mLabelTexts[CharackBenchmark::LABEL_COASTLINE_GENERATION],		"Coastline generation");
	strcpy(mLabelTexts[CharackBenchmark::LABEL_BEACH_GENERATION],			"Beach generation");
	strcpy(mLabelTexts[CharackBenchmark::LABEL_TERRAIN_SMOOTHING],			"Smoothing terrain data");
	strcpy(mLabelTexts[CharackBenchmark::LABEL_RENDERIZATION],				"Time to render");
	strcpy(mLabelTexts[CharackBenchmark::LABEL_OCEAN_HEIGHTMAP],			"Ocean heightmap");
}

void CharackBenchmark::startClock() {
	mStartMark = GetTickCount();
}

void CharackBenchmark::stopClock(char theLabel) {
	long int aTotalTime = GetTickCount() - mStartMark;
	mTimeCount[theLabel] += aTotalTime;
}

void CharackBenchmark::printReport(char *thePath) {
	FILE *aFile;
	long int aTotalTime = 0;

	aFile = fopen(thePath, "w");

	if (aFile == NULL) {
		fprintf(stderr, "CharackBenchmark: could not open output file %s\n", thePath);
		exit(0);
	} else {
		fprintf(aFile, "---------------------------------------------------------------\n");
		fprintf(aFile, "-                   Charack  benchmark                        -\n");
		fprintf(aFile, "---------------------------------------------------------------\n");
		fprintf(aFile, "\n");

		fprintf(aFile, "Charack configuration:\n");
		fprintf(aFile, "	World slice size: %dx%d vertexes\n", CK_DIM_TERRAIN, CK_DIM_TERRAIN);
		fprintf(aFile, "	MM size: %dx%d pixels\n", CK_MACRO_MATRIX_WIDTH, CK_MACRO_MATRIX_HEIGHT);
		fprintf(aFile, "	World maximum size: %.1fx%.1f vertexes\n", CK_MAX_WIDTH, CK_MAX_WIDTH);
		
		fprintf(aFile, "\nTime per task:\n");
		
		for(char i = 0; i < CK_BENCH_MAX_LABELS; i++) {
			fprintf(aFile, "	%s: %ld ms\n", mLabelTexts[i], mTimeCount[i]);
			aTotalTime += mTimeCount[i];
		}
		
		aTotalTime = aTotalTime - mTimeCount[CharackBenchmark::LABEL_MM_GENERATION];
		fprintf(aFile, "\n	TOTAL (excluding MM generation): %ld ms\n", aTotalTime);

		fprintf(aFile, "\nOverall:\n\n");
		fprintf(aFile, "Charack takes %ld ms to initiate all variables and parameters. After that, it takes %ld ms to generate and render a scene containing %d vertexes.", mTimeCount[CharackBenchmark::LABEL_MM_GENERATION], aTotalTime, CK_DIM_TERRAIN*CK_DIM_TERRAIN);

		fclose(aFile);
	}
}