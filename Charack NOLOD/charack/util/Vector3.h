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

class Vector3  
{
public:

	float x, y, z, a;

	Vector3();
	Vector3(float vx, float vy, float vz);
	Vector3(float vx, float vy, float vz,  float va);
	Vector3(const Vector3& u);
	virtual ~Vector3();
	void set(float vx, float vy, float vz);
	void set(Vector3 v);

	Vector3& operator-= (const Vector3& v2);
	Vector3& operator+= (const Vector3& v2);
	Vector3& operator*= (float f);
	Vector3  operator / (float f);
    Vector3  operator + (const Vector3& v);
    Vector3  operator - (const Vector3& v);
    Vector3  operator ^ (const Vector3& v);
    Vector3  operator * (const float s);

    void rotate2D(const float angle);
	float dot(const Vector3 &v);
	float dot2D(const Vector3 &v);
    float angle2D(const Vector3 &v);
    int side(const Vector3 &v);
	void normalize();
	void normalize2D(char *caller);
	void rotatex(float angle);
	void rotatey(float angle);
	void rotatez(float angle);
	void rotate(float rx, float ry, float rz);
	float distance2D(const Vector3& v) const;
	Vector3 perp2D() const;
	float lenghtSqr2D();
	void truncate2D(float max);

	void print(void);
	void print(char *msg);

   char* Vector3::toString(void);
};

#endif
