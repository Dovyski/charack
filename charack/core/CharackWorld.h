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

#ifndef _CHARACK_RENDER_H_
#define _CHARACK_RENDER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

class CharackWorldSlice;

#include "CharackObserver.h"
#include "CharackMapGenerator.h"
#include "CharackCamera.h"
#include "CharackTerrain.h"
#include "CharackWorldSlice.h"

#include "../util/perlin.h"
#include "../util/imageloader.h"

class CharackWorld {
	private:
		CharackObserver *mObserver;
		CharackMapGenerator *mMapGenerator;
		CharackCamera *mCamera;
		CharackTerrain *mTerrain;
		CharackWorldSlice *mWorldSlice;
		CharackCoastGenerator *mCoastGen;

		float (*mHeightFunctionX)(float); // generate the height coordinates for X axis
		float (*mHeightFunctionZ)(float); // generate the height coordinates for X axis
		
		int mViewFrustum;
		int mSample;
		float mScale;
		Perlin *mPerlinNoise;
		GLuint mTextureId; //The id of the texture (ocean)

		CharackVector3 calculateNormal(CharackVector3 theLeftPoint, CharackVector3 theMiddlePoint, CharackVector3 theRightPoint);
		GLuint CharackWorld::loadTexture(Image* theImage);

	public:
		CharackWorld(int theViewFrustum, int theSample);
		~CharackWorld();

		void displayMap(void);
		float getHeight(float theX, float theZ);
		float getHeightAtObserverPosition(void);
		void generateMap(void);
		CharackObserver *getObserver(void);
		CharackMapGenerator *getMapGenerator(void);
		CharackCamera *getCamera(void);
		CharackTerrain *getTerrain(void);
		CharackCoastGenerator *getCoastGenerator(void);

		void setViewFrustum(int theViewFrustum);
		int getViewFrustum();

		void setSample(int theViewFrustum);
		int getSample();

		void setScale(float theScale);
		float getScale();

		CharackWorldSlice *getWorldSlice(void);

		void setHeightFunctionX(float (*theFunction)(float));
		void setHeightFunctionZ(float (*theFunction)(float));

		// Print useful information about the world.
		void printDebugInfo(void);

		void placeObserverOnLand(void);
		int update(void);
		void renderReferenceAxis(void);
		void renderWorldMap(int theScreenWidth, int theScreenHeight);
		void renderOcean(void);
		void renderFog(void);

		void init(void);
};

#endif