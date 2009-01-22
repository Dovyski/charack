#ifndef __CHARACK_COST_GENERATOR_H_
#define __CHARACK_COST_GENERATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>

#include "config.h"
#include "../tools/vector3.h"

#define _CK_CG_RRAND(a, b) (int)((a) + rand()/(RAND_MAX + 1.0) * ((b) - (a) + 1))

/**
 * Generates the coast for a continent. Using the midpoint displacement fractal, the class can transform
 * a straight line into a set of disturbed points, which can be used to render a continent coast.
 */
class CharackCoastGenerator {
	private:
		int mMaxDivision;
		int mMaxVariation;

	public:
		static enum CLASS_DEFS {
			AXIS_X,
			AXIS_Y,
			AXIS_Z
		};

		CharackCoastGenerator();
		~CharackCoastGenerator();

		void setMaxDivisions(int theHowMany);
		int getMaxDivisions();

		void setVariation(int theHowMuch);
		int getVariation();

		void setRandSeed(int theSeed);

		std::list<Vector3> generate(Vector3 thePointA, Vector3 thePointB, int thePerturbationAxis);
};

#endif