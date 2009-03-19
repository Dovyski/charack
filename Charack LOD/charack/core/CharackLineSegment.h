#ifndef __CHARACK_LINE_SEGMENT_H_
#define __CHARACK_LINE_SEGMENT_H_

#include "../tools/vector3.h"
#include "config.h"

class CharackLineSegment {
	private:
		Vector3 mPointA;
		Vector3 mPointB;
		int mOrientationAxis;

	public:
		static enum CLASS_DEFS {
			AXIS_X,
			AXIS_Y,
			AXIS_Z
		};

		CharackLineSegment(Vector3 thePointA, Vector3 thePointB, int theOrientationAxis);
		~CharackLineSegment();

		Vector3 getPointA();
		Vector3 getPointB();
		int getOrientationAxis();
};

#endif