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
	computedNormals = false;
	mPerlinNoise	= new Perlin(16, 8, 0.25, 20);
}
		
CharackTerrain::~CharackTerrain() {
}
		

void CharackTerrain::setHeight(int x, int z, float y) {
	hs[z][x] = y;
	computedNormals = false;
}


float CharackTerrain::getHeight(int x, int z) {
	return hs[z][x];
}

void CharackTerrain::computeNormals() {
	if (computedNormals) {
		return;
	}
	
	//Compute the rough version of the normals
	Vec3f normals2[CK_DIM_TERRAIN][CK_DIM_TERRAIN];
	
	for(int z = 0; z < CK_DIM_TERRAIN; z++) {
		for(int x = 0; x < CK_DIM_TERRAIN; x++) {
			Vec3f sum(0.0f, 0.0f, 0.0f);
			
			Vec3f out;
			if (z > 0) {
				out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
			}
			Vec3f in;
			if (z < CK_DIM_TERRAIN - 1) {
				in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
			}
			Vec3f left;
			if (x > 0) {
				left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
			}
			Vec3f right;
			if (x < CK_DIM_TERRAIN - 1) {
				right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
			}
			
			if (x > 0 && z > 0) {
				sum += out.cross(left).normalize();
			}
			if (x > 0 && z < CK_DIM_TERRAIN - 1) {
				sum += left.cross(in).normalize();
			}
			if (x < CK_DIM_TERRAIN - 1 && z < CK_DIM_TERRAIN - 1) {
				sum += in.cross(right).normalize();
			}
			if (x < CK_DIM_TERRAIN - 1 && z > 0) {
				sum += right.cross(out).normalize();
			}
			
			normals2[z][x] = sum;
		}
	}
	
	//Smooth out the normals
	const float FALLOUT_RATIO = 0.5f;
	for(int z = 0; z < CK_DIM_TERRAIN; z++) {
		for(int x = 0; x < CK_DIM_TERRAIN; x++) {
			Vec3f sum = normals2[z][x];
			
			if (x > 0) {
				sum += normals2[z][x - 1] * FALLOUT_RATIO;
			}
			if (x < CK_DIM_TERRAIN - 1) {
				sum += normals2[z][x + 1] * FALLOUT_RATIO;
			}
			if (z > 0) {
				sum += normals2[z - 1][x] * FALLOUT_RATIO;
			}
			if (z < CK_DIM_TERRAIN - 1) {
				sum += normals2[z + 1][x] * FALLOUT_RATIO;
			}
			
			if (sum.magnitude() == 0) {
				sum = Vec3f(0.0f, 1.0f, 0.0f);
			}
			normals[z][x] = sum;
		}
	}
	
	computedNormals = true;
}


Vec3f CharackTerrain::getNormal(int x, int z) {
	if (!computedNormals) {
		computeNormals();
	}
	return normals[z][x];
}

void CharackTerrain::loadData(float *theData) {
	int i = 0;
	for(int y = 0; y < CK_DIM_TERRAIN; y++) {
		for(int x = 0; x < CK_DIM_TERRAIN; x++) {
			setHeight(x, y, theData[i++]);
		}
	}
	
	computeNormals();
}

void CharackTerrain::makeDataSmooth(int theHowManyTimes) {
	float aTemp[CK_DIM_TERRAIN + 2][CK_DIM_TERRAIN + 2];
	int aSizeTemp = CK_DIM_TERRAIN + 2;

	theHowManyTimes = theHowManyTimes <= 0 ? 1 : theHowManyTimes;

	while(theHowManyTimes--) {
		// Lets duplicate the heightmap
		for(int y = 1; y <= CK_DIM_TERRAIN; y++) {
			for(int x = 1; x <= CK_DIM_TERRAIN; x++) {
				aTemp[x][y] = hs[x-1][y-1];
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

				hs[x-1][y-1] = aCenter + aSides + aCorners;
			}
		}
	}

	computeNormals();
}


void CharackTerrain::render(float theScale) {
	float scale = theScale / max(CK_DIM_TERRAIN - 1, CK_DIM_TERRAIN - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(CK_DIM_TERRAIN - 1) / 2,
				 0.0f,
				 -(float)(CK_DIM_TERRAIN - 1) / 2);
	
	applyColorByHeight(0, 0, 0);
	for(int z = 0; z < CK_DIM_TERRAIN - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < CK_DIM_TERRAIN; x++) {
			Vec3f normal = getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			applyColorByHeight(x*CK_MESH_SPACE, getHeight(x, z), z * CK_MESH_SPACE);
			glVertex3f(x*CK_MESH_SPACE, getHeight(x, z), z * CK_MESH_SPACE);

			normal = getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			applyColorByHeight(x*CK_MESH_SPACE, getHeight(x, z + 1), (z + 1)*CK_MESH_SPACE);
			glVertex3f(x*CK_MESH_SPACE, getHeight(x, z + 1), (z + 1)*CK_MESH_SPACE);
		}
		glEnd();
	}
}

void CharackTerrain::applyColorByHeight(float theX, float theY, float theZ) {
	if(theY > (CK_MAX_HEIGHT * 0.50)) {
		glColor3f(1.0f, abs(mPerlinNoise->Get(0.45, theY)) + 0.8f, 1.0f);

	} else if(theY > (CK_MAX_HEIGHT * 0.30)) {
		glColor3f(0.0f, abs(mPerlinNoise->Get(0.45, theY)) + 0.2f, 0.0f);

	} else if(theY > (CK_MAX_HEIGHT * 0.10)) {
		glColor3f(0.0f, abs(mPerlinNoise->Get(0.45, theY)) + 0.3f, 0.0f);

	} else if(theY > (CK_MAX_HEIGHT * 0.05)) {
		glColor3f(0.0f, abs(mPerlinNoise->Get(0.45, theY)) + 0.4f, 0.0f);

	} else {
		glColor3f(0.0f, 0.0f, abs(mPerlinNoise->Get(0.45, theY)) + 0.2f);
	}
}