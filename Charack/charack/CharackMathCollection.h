#ifndef __CHARACK_MATH_COLLETION_H_
#define __CHARACK_MATH_COLLETION_H_

#include <stdio.h>

#define CK_MATHC_MAX_FUNCTION	4

/**
 * Manage a collection of math function, providing a easy way to get a interpolation of them.
 */
class CharackMathCollection {
	private:
		float (*mFunctions[CK_MATHC_MAX_FUNCTION])(float); // array of pointers to functions prototyped as "float f(float)".
		float mWeights[CK_MATHC_MAX_FUNCTION];

		int getNextAvailablePos();

	public:
		CharackMathCollection();		
		~CharackMathCollection();

		bool addFunction(float (*theFunction)(float));
		bool removeFunction(float (*theFunction)(float));
		void setWeights(float theWeights[]);
		float getValue(float theThreshold);
};

#endif