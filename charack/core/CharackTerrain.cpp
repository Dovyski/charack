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


/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* File for Terrain lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */

#include "CharackTerrain.h"

CharackTerrain::CharackTerrain() {
	mHeightData = NULL;
}
		
CharackTerrain::~CharackTerrain() {
}

void CharackTerrain::loadData(float *theData) {
	mHeightData = theData;
}

float *CharackTerrain::getData() {
	return mHeightData;
}

void CharackTerrain::makeDataSmooth(int theHowManyTimes) {
	float aTemp[CK_DIM_TERRAIN + 2][CK_DIM_TERRAIN + 2];
	int aSizeTemp = CK_DIM_TERRAIN + 2;

	theHowManyTimes = theHowManyTimes <= 0 ? 1 : theHowManyTimes;

	while(theHowManyTimes--) {
		// Lets duplicate the heightmap
		for(int y = 1; y <= CK_DIM_TERRAIN; y++) {
			for(int x = 1; x <= CK_DIM_TERRAIN; x++) {
				aTemp[x][y] = mHeightData[(x-1)*CK_DIM_TERRAIN + (y-1)];
			}
		}

		// Make it seamless
		for (int x=1; x < (aSizeTemp -1); x++) {
			aTemp[0			   ][x			  ] = aTemp[1][x];
			aTemp[aSizeTemp - 1][x			  ] = aTemp[aSizeTemp - 2][x];
			aTemp[x			   ][0			  ] = aTemp[x][1];
			aTemp[x			   ][aSizeTemp - 1] = aTemp[x][aSizeTemp - 2];
		}

		aTemp[0			   ][0			  ]	= aTemp[1][1];
		aTemp[aSizeTemp - 1][aSizeTemp - 1] = aTemp[aSizeTemp - 2][aSizeTemp - 2];
		aTemp[0			   ][aSizeTemp - 1]	= aTemp[1][aSizeTemp - 2];
		aTemp[aSizeTemp - 1][0			  ]	= aTemp[aSizeTemp - 2][1];


		// Smooth them all
		for(int y = 1; y < (aSizeTemp - 1); y++) {
			for(int x = 1; x < (aSizeTemp - 1); x++) {
				float aCenter = aTemp[x][y]/4.0f;
				float aSides = (aTemp[x+1][y] + aTemp[x-1][y] + aTemp[x][y+1] + aTemp[x][y-1])/8.0f;
				float aCorners = (aTemp[x+1][y+1] + aTemp[x+1][y-1] + aTemp[x-1][y+1] + aTemp[x-1][y-1])/16.0f;

				mHeightData[(x-1)*CK_DIM_TERRAIN + (y-1)] = aCenter + aSides + aCorners;
			}
		}
	}
}