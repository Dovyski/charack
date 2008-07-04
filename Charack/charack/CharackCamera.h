#ifndef __CHARACK_CAMERA_H_
#define __CHARACK_CAMERA_H_

#include <math.h>
#include "vector3.h"
#include "config.h"

class CharackCamera {
	private:
		int mRotX;
		int mRotY;
		Vector3 *mPosition;

	public:
		CharackCamera();		
		~CharackCamera();

		void moveForward(int theHowMuch);
		void moveBackward(int theHowMuch);
		void moveLeft(int theHowMuch);
		void moveRight(int theHowMuch);

		void rotateLookLeftRight(int theDegres);
		void rotateLookUpDown(int theDegres);

		float getPositionX();
		float getPositionY();
		float getPositionZ();

		Vector3 *getPosition();

		int getRotationX();
		int getRotationY();
};

#endif