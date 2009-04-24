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

#ifndef __CHARACK_COST_GENERATOR_H_
#define __CHARACK_COST_GENERATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

class CharackMapGenerator;
class CharackObserver;

#include "CharackMapGenerator.h"
#include "CharackObserver.h"

#include "config.h"
#include "../util/vector3.h"
#include "../util/perlin.h"

#define _CK_CG_GET(theMatrix, i, j)					(theMatrix[(i)*(CK_DIM_TERRAIN) + (j)])
#define _CK_CG_CONSTRAINT_CHECK(z)					(z < CK_MAX_WIDTH)

/**
 * Generates the coast for a continent. Using the midpoint displacement fractal, the class can transform
 * a straight line into a set of disturbed points, which can be used to render a continent coast.
 */
class CharackCoastGenerator {
	private:
		Perlin *mPerlinNoise;
		int mMaxStepsLand;
		int mMaxStepsWater;
		float mMaxBeachHeight;

		float generateBeachHeight(float theTotalDistance);
		float generateDisturbedCoastLineHeight(float theTotalDistance, float theXObserver, float theZObserver);

	public:
		CharackCoastGenerator();
		~CharackCoastGenerator();

		void setMaxStepsLand(int theValue);
		int getMaxStepsLand();

		void setMaxStepsWater(int theValue);
		int getMaxStepsWater();

		void setMaxBeachHeight(float theValue);
		float getMaxBeachHeight();

		void disturbStraightCoastLines(float *theHeightData, CharackMapGenerator *theMapGenerator, CharackObserver *theObserver, int theSample);
};

#endif