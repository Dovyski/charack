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
#include "../tools/vector3.h"
#include "../defs.h"

#define _CK_CG_GET(theMatrix, i, j)					(theMatrix[(i)*(DIM_TERRAIN + 1) + (j)])
#define _CK_CG_CONSTRAINT_CHECK(z)					(z < CK_MAX_WIDTH)

/**
 * Generates the coast for a continent. Using the midpoint displacement fractal, the class can transform
 * a straight line into a set of disturbed points, which can be used to render a continent coast.
 */
class CharackCoastGenerator {
	private:
		int mMaxSteps;
		int mMaxBeachHeight;
		int distanceFromWater(CharackMapGenerator *theMapGenerator, float theXObserver, float theZObserver, int theSample, int theDirection);

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

		void disturbStraightCoastLines(unsigned char *theHeightData, CharackMapGenerator *theMapGenerator, CharackObserver *theObserver, int theSample);
};

#endif