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

#include "CharackWorld.h"
#include "../util/imageloader.h"

CharackWorld::CharackWorld(int theViewFrustum, int theSample) {
	mObserver		= new CharackObserver();
	mCamera			= new CharackCamera();
	mMapGenerator	= new CharackMapGenerator();
	mWorldSlice		= new CharackWorldSlice(this);
	mPerlinNoise	= new Perlin(16, 8, 1, 10);
	mCoastGen		= new CharackCoastGenerator();
	mTerrain		= new CharackTerrain(CK_DIM_TERRAIN, CK_DIM_TERRAIN);
	
	// Generate the world. The mMapGenerator is our "guide", it genetares the huge things in the
	// world, like oceans and continents, then the other Charack classes will use that "guide"
	// as a clue repository to generate specific height variation, beach stuff, mountains, etc.
	mMapGenerator->generate();
	
	setViewFrustum(theViewFrustum);
	setSample(theSample);
	setScale(CK_SCALE);
}

CharackWorld::~CharackWorld() {
}

void CharackWorld::render(void) {
	getCamera()->render();

	if(getWorldSlice()->updateData()) {
		// The slice has changed. We must update the terrain mesh
		// and all its friends.

		// Before we send the terrain info to the LOD manager, we must edit
		// the info matrix to insert the water information and, after that,
		// make the coast lines a litte bit more realistic.
		// So, lets do that:
		getCoastGenerator()->disturbStraightCoastLines(getWorldSlice()->getHeightData(), getMapGenerator(), getObserver(), getSample());

		// All information we have is smooth and ready to be displayed. 
		// Lets update the terrain mesh...
		getTerrain()->loadData(getWorldSlice()->getHeightData());

		// Make all data smooth to avoid sharp corners 
		getTerrain()->makeSmooth();
	}

	getTerrain()->render(getScale());
}

void CharackWorld::renderReferenceAxis() {
	int aLength = 200;

	GLfloat X = 0.0f; // Translate screen to x direction (left or right)
	GLfloat Y = 0.0f; // Translate screen to y direction (up or down)
	GLfloat Z = 0.0f; // Translate screen to z direction (zoom in or out)
	GLfloat rotX = 0.0f; // Rotate screen on x axis 
	GLfloat rotY = 0.0f; // Rotate screen on y axis
	GLfloat rotZ = 0.0f; // Rotate screen on z axis
	GLfloat rotLx = 0.0f; // Translate screen by using the glulookAt function (left or right)
	GLfloat rotLy = 0.0f; // Translate screen by using the glulookAt function (up or down)
	GLfloat rotLz = 0.0f; // Translate screen by using the glulookAt function (zoom in or out)

    glPushMatrix(); // It is important to push the Matrix before calling glRotatef and glTranslatef
    glRotatef(rotX,1.0,0.0,0.0); // Rotate on x
    glRotatef(rotY,0.0,1.0,0.0); // Rotate on y
    glRotatef(rotZ,0.0,0.0,1.0); // Rotate on z
    glTranslatef(X, Y, Z); // Translates the screen left or right, up or down or zoom in zoom out
    // Draw the positive side of the lines x,y,z

	glLineWidth(5);

    glBegin(GL_LINES);
    glColor3f (0.0, 1.0, 0.0); // Green for x axis
    glVertex3f(0,0,0);
    glVertex3f(aLength,0,0);
    glColor3f(1.0,0.0,0.0); // Red for y axis
    glVertex3f(0,0,0);
    glVertex3f(0,aLength,0);
    glColor3f(0.0,0.0,1.0); // Blue for z axis
    glVertex3f(0,0,0); 
    glVertex3f(0,0,aLength);
    glEnd();

    // Dotted lines for the negative sides of x,y,z
    glEnable(GL_LINE_STIPPLE); // Enable line stipple to use a dotted pattern for the lines
    glLineStipple(1, 0x0101); // Dotted stipple pattern for the lines
    glBegin(GL_LINES); 
    glColor3f (0.0, 1.0, 0.0); // Green for x axis
    glVertex3f(-aLength,0,0);
    glVertex3f(0,0,0);
    glColor3f(1.0,0.0,0.0); // Red for y axis
    glVertex3f(0,0,0);
    glVertex3f(0,-aLength,0);
    glColor3f(0.0,0.0,1.0); // Blue for z axis
    glVertex3f(0,0,0);
    glVertex3f(0,0,-aLength);
    glEnd();
    glDisable(GL_LINE_STIPPLE); // Disable the line stipple
}

void CharackWorld::renderOcean() {
	float aLength = 2000;
	float aHeight = 20;
	
    glColor3f (0.0, 0.0, 1.0);

	glBegin(GL_QUADS);
	glVertex3f(0.0f, aHeight, 0.0f);
	glVertex3f(0.0f, aHeight, aLength);
	glVertex3f(aLength, aHeight, aLength);
	glVertex3f(aLength, aHeight, 0.0f);
	glEnd();
}


float CharackWorld::getHeight(float theX, float theZ) {
	if(CK_COAST_MANGLE_HEIGHT) {
		int aLimit = CK_MAX_HEIGHT - CK_COAST_BEACH_HEIGHT;
		return CK_COAST_BEACH_HEIGHT + abs(mPerlinNoise->Get(theX/200, theZ/200) * aLimit);
	} else {
		return abs(mPerlinNoise->Get(theX/200, theZ/200)) * CK_MAX_HEIGHT;
	}
}

float CharackWorld::getHeightAtObserverPosition(void) {
	return getHeight(getObserver()->getPosition()->x, getObserver()->getPosition()->z);
}

CharackObserver *CharackWorld::getObserver(void) {
	return mObserver;
}


// This method will perform the following: (theLeftPoint - theMiddlePoint) ^ (theRightPoint - theMiddlePoint)
Vector3 CharackWorld::calculateNormal(Vector3 theLeftPoint, Vector3 theMiddlePoint, Vector3 theRightPoint) {
	Vector3 aV0, aV1, aNormal;

	aV0 = theLeftPoint - theMiddlePoint;
	aV1 = theRightPoint - theMiddlePoint;
	aNormal = aV1 ^ aV0;
	aNormal.normalize();

	return aNormal;
}

void CharackWorld::setViewFrustum(int theViewFrustum) {
	mViewFrustum = (theViewFrustum < 0 || theViewFrustum > CK_VIEW_FRUSTUM) ? CK_VIEW_FRUSTUM : theViewFrustum; 
}

int CharackWorld::getViewFrustum() {
	return mViewFrustum;
}

void CharackWorld::printDebugInfo(void) {
	printf("--- Charack World (Debug info) ---\n\n");
	printf("Observer = (%d,%d,%d), [rotx, roty] = (%d, %d)\n", (int)getObserver()->getPositionX(), (int)getObserver()->getPositionY(), (int)getObserver()->getPositionZ(), getObserver()->getRotationX(), getObserver()->getRotationY());	
	printf("View frustum = %d\n", getViewFrustum());
	printf("Sample = %d\n", getSample());
	printf("Scale = %.2f\n", getScale());
	printf("Terrain height (observer) = %.2f\n", getHeightAtObserverPosition());
	printf("isLand: %d\n", getMapGenerator()->isLand(getObserver()->getPositionX(), getObserver()->getPositionZ()));
	printf("\nControls:\n");
	printf("\t Move: w,a,s,d\n");
	printf("\t Turn: q,e\n");
	printf("\t Curve: r,f\n");
	printf("\t Move Up/Down: t,g\n");
	printf("\t View frustum: c,v\n");
	printf("\t Sampling: n,m\n");
	printf("\t Scale: k,l\n");
}

void CharackWorld::placeObserverOnLand() {
	for(int z=0; z < CK_MAX_WIDTH; z++) {
		for(int x=0; x < CK_MAX_WIDTH; x++) {
			if(getMapGenerator()->isLand(x, z)) {
				getObserver()->setPosition(x, getObserver()->getPositionY(), z);
				return;
			}
		}
	}
}


void CharackWorld::setSample(int theSample) {
	mSample = theSample <= 0 ? 1 : theSample;
}

int CharackWorld::getSample() {
	return mSample;
}

void CharackWorld::setScale(float theScale) {
	mScale = theScale;
}

float CharackWorld::getScale() {
	return mScale;
}

CharackMapGenerator *CharackWorld::getMapGenerator(void) {
	return mMapGenerator;
}


void CharackWorld::setHeightFunctionX(float (*theFunction)(float)) {
	mHeightFunctionX = theFunction;
}

void CharackWorld::setHeightFunctionZ(float (*theFunction)(float)) {
	mHeightFunctionZ = theFunction;
}

CharackCamera *CharackWorld::getCamera(void) {
	return mCamera;
}

CharackTerrain *CharackWorld::getTerrain(void) {
	return mTerrain;
}

CharackWorldSlice *CharackWorld::getWorldSlice(void) {
	return mWorldSlice;
}

CharackCoastGenerator *CharackWorld::getCoastGenerator(void) {
	return mCoastGen;
}