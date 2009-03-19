#ifndef _VLFRUSTUM_H
#define _VLFRUSTUM_H	

#include "../tools/Vector3.h"

class CharackFrustrum  
{
public:
	CharackFrustrum(Vector3 cameraPosition, Vector3 cameraDirection, float angle);
	virtual ~CharackFrustrum();

	bool visible(int xmin, int ymin, int xmax, int ymax);
	//float distanceFromCameraSquared(int x, int y);
	//Vector3 getCameraPosition() { return cameraPosition; }

private:
	float angle;
	//float aspectRatio;
	Vector3 cameraPosition;
	Vector3 cameraDirection;
	float frustrumLX, frustrumLY, frustrumRX, frustrumRY;
};

#endif
