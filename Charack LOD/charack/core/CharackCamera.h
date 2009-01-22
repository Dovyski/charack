//-------------------------------------------------------------------------
// classe para definicao da camera 
// by Cesar Pozzer 
//-------------------------------------------------------------------------

#ifndef _CHARACKCAMERA_H
#define _CHARACKCAMERA_H	

#include "../tools/Vector3.h"
#include <stdio.h>

class CharackCamera{
public:
	//posicao, direcao e velocidade da camera
	Vector3 camPos, camDir;
	Vector3 camLockPos, camLockDir;
	Vector3 camPosOld, camDirOld;

	//parametros de projecao da camera
	float abertura, znear, zfar, aspect;
	float camVel;
	
	int mTopView;

	CharackCamera();

	void rotate(float rx, float ry, float rz);
	void walk(float dist);
	void move(float dist);
	void elevate(float dist);
	void topView(int theStatus);

	void render();
	void lockPosition();
	bool visible(int xmin, int ymin, int xmax, int ymax);

private:
	float frustrumLX, frustrumLY, frustrumRX, frustrumRY;
	bool lock;

	void  setupCamera();
	void  setupFrustum();

};

#endif
