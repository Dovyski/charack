#ifndef __CHARACK_OBSERVER_H_
#define __CHARACK_OBSERVER_H_

#include <math.h>
#include "vector3.h"
#include "config.h"

class CharackObserver {
	private:
		int mRotX;
		int mRotY;
		Vector3 *mPosition;

	public:
		CharackObserver();		
		~CharackObserver();

		void moveForward(int theHowMuch);
		void moveBackward(int theHowMuch);
		void moveLeft(int theHowMuch);
		void moveRight(int theHowMuch);

		void moveUpDown(int theHowMuch);

		void rotateLookLeftRight(int theDegres);
		void rotateLookUpDown(int theDegres);

		float getPositionX();
		float getPositionY();
		float getPositionZ();

		void setPosition(float theX, float theY, float theZ);

		Vector3 *getPosition();

		int getRotationX();
		int getRotationY();

		void setRotationX(int theRotation);
		void setRotationY(int theRotation);
};

#endif