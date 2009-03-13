#ifndef __CHARACK_COST_GENERATOR_H_
#define __CHARACK_COST_GENERATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "../tools/vector3.h"
#include "../defs.h"

#define _CK_CG_GET(theMatrix, i, j)					(theMatrix[(i)*(DIM_TERRAIN + 1) + (j)])
#define _CK_CG_CONSTRAINT_CHECK(z)					(z >= 0 && z < (DIM_TERRAIN + 1))
#define _CK_CG_CALCULATE_I(theIndex)				(int)floor((theIndex)/(DIM_TERRAIN + 1.))
#define _CK_CG_CALCULATE_J(theIndex)				(int)floor((theIndex/(DIM_TERRAIN + 1.) - floor(theIndex/(DIM_TERRAIN + 1.)))*(DIM_TERRAIN + 1.))

/**
 * Generates the coast for a continent. Using the midpoint displacement fractal, the class can transform
 * a straight line into a set of disturbed points, which can be used to render a continent coast.
 */
class CharackCoastGenerator {
	private:
		int mMaxSteps;
		int mMaxBeachHeight;
		int distanceFromWater(unsigned char *theLandAndWaterData, int theIndex, int theDirection);

	public:
		static const int MOVE_RIGHT		= 1;
		static const int MOVE_LEFT		= -1;
		static const int MOVE_UP		= -1;
		static const int MOVE_DOWN		= 1;

		CharackCoastGenerator();
		~CharackCoastGenerator();

		void setMaxSteps(int theValue);
		int getMaxSteps();

		void setMaxBeachHeight(int theValue);
		int getMaxBeachHeight();

		void disturbStraightCoastLines(unsigned char *theHeightData,unsigned char *theLandAndWaterData);
};

#endif