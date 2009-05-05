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

#ifndef _VECTOR3_H
#define _VECTOR3_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef PI
	#define PI 3.14159265358979
#endif

#ifndef PIOVER180
	#define PIOVER180 0.0174532925f
#endif

#ifndef PIOVER180
	#define PI2       PI/2
#endif

class CharackVector3  
{
public:

	float x, y, z, a;

	CharackVector3();
	CharackVector3(float vx, float vy, float vz);
	CharackVector3(float vx, float vy, float vz,  float va);
	CharackVector3(const CharackVector3& u);
	virtual ~CharackVector3();
	void set(float vx, float vy, float vz);
	void set(CharackVector3 v);

	CharackVector3& operator-= (const CharackVector3& v2);
	CharackVector3& operator+= (const CharackVector3& v2);
	CharackVector3& operator*= (float f);
	CharackVector3  operator / (float f);
    CharackVector3  operator + (const CharackVector3& v);
    CharackVector3  operator - (const CharackVector3& v);
    CharackVector3  operator ^ (const CharackVector3& v);
    CharackVector3  operator * (const float s);

    void rotate2D(const float angle);
	float dot(const CharackVector3 &v);
	float dot2D(const CharackVector3 &v);
    float angle2D(const CharackVector3 &v);
    int side(const CharackVector3 &v);
	void normalize();
	void normalize2D(char *caller);
	void rotatex(float angle);
	void rotatey(float angle);
	void rotatez(float angle);
	void rotate(float rx, float ry, float rz);
	float distance2D(const CharackVector3& v) const;
	CharackVector3 perp2D() const;
	float lenghtSqr2D();
	void truncate2D(float max);

	void print(void);
	void print(char *msg);

   char* CharackVector3::toString(void);
};

#endif
