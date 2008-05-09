// Vector3.h: interface for the Vector3 class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _VECTOR3_H
#define _VECTOR3_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define PIOVER180 0.0174532925f
#define PI        3.1415
#define PI2       PI/2

class Vector3  
{
public:

	float x, y, z;

	Vector3();
	Vector3(float vx, float vy, float vz);
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
