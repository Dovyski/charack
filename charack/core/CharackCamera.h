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

#ifndef _CHARACKCAMERA_H
#define _CHARACKCAMERA_H	

#include "../util/CharackVector3.h"
#include <stdio.h>

class CharackCamera{
public:
	//posicao, direcao e velocidade da camera
	CharackVector3 camPos, camDir;
	CharackVector3 camLockPos, camLockDir;
	CharackVector3 camPosOld, camDirOld;

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
