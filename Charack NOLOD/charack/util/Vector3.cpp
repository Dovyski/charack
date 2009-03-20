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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Vector3.h"


//----------------------------------------------------------------------
// Construction/Destruction
//----------------------------------------------------------------------

Vector3::Vector3() 
{ 
   x = y = z = a = 0;    
}

Vector3::Vector3(float vx, float vy, float vz) 
{
   x = vx;  y = vy;  z = vz; 
}

Vector3::Vector3(float vx, float vy, float vz,  float va) 
{
   x = vx;  y = vy;  z = vz;  a = va;
}

Vector3::Vector3(const Vector3& u)
{
	//printf("\ncopia");
   x = u.x; 
	y = u.y; 
	z = u.z;
}

Vector3::~Vector3()
{

}

void Vector3::set( const float vx, const float vy, const float vz )
{
	x = vx;
	y = vy;
	z = vz;						
};

void Vector3::set(const Vector3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;						
};


//----------------------------------------------------------------------
// OPERATORS OPERATORS OPERATORS OPERATORS
//----------------------------------------------------------------------
Vector3& Vector3::operator-= (const Vector3& v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

Vector3& Vector3::operator+= (const Vector3& v)
{
	x += v.x; y += v.y; z += v.z;
	return *this;
}


Vector3& Vector3::operator*= (float f)
{
	x *= f; y *= f; z *= f;
	return *this;
}

Vector3 Vector3::operator + ( const Vector3& v )
{
	Vector3 aux( x + v.x, y + v.y, z + v.z );
	return( aux );
}

Vector3 Vector3::operator / (float f)
{
	Vector3 aux( x/f, y/f, z/f );
	return( aux );
}

//	Subtracao de vetores
Vector3 Vector3::operator - ( const Vector3& v )
{
	Vector3 aux( x - v.x, y - v.y, z - v.z );
	return( aux );
}

// cross
Vector3 Vector3::operator ^ (const Vector3& v)
{
	Vector3 aux( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
	return( aux );
}

//	Produto por escalar (float)
Vector3 Vector3::operator * ( const float s )
{
	Vector3 aux( x * s, y * s, z * s );
	return( aux );
}

float Vector3::dot(const Vector3 &v)
{
    return x*v.x + y*v.y + z*v.z;
}

float Vector3::dot2D(const Vector3 &v)
{
    return x*v.x + z*v.z;
}

float Vector3::angle2D(const Vector3 &v)
{
    float dp, angPI ;
    
    dp = dot2D(v); //dot product
    if(dp >= 1.0) dp = 1.0f;
    if(dp <=-1.0) dp =-1.0f;
    
    angPI = (float)acos(dp);
    
    //determina a posicao relativa do vetor
    return angPI * side(v);
}

int Vector3::side(const Vector3 &v)
{
    if (z*v.x > x*v.z)
        return -1;
    else
        return 1;
}

void Vector3::normalize()
{
	float norm = (float)sqrt(x*x + y*y + z*z);

	if(norm==0.0)
	{
		printf("\n\nNormalize::Divisao por zero");
	   x = 1;
	   y = 1;
	   z = 1;
      return;
		exit(1);
	}
	x /= norm;
	y /= norm;
	z /= norm;
}

void Vector3::normalize2D(char *caller)
{
	float norm = (float)sqrt(x*x + z*z);

	if(norm==0.0)
	{
		printf("\n\nNormalize 2D: Divisao por zero: %s", caller);
	   x = 1;
	   y = 0;
	   z = 1;
      print();
      return;
		exit(1);
	}
	x /= norm;
	y = 0.0;
	z /= norm;
}

void Vector3::rotate2D(const float angle)
{
    float c = (float)cos(angle);
    float s = (float)sin(angle);
    float x2 = x*c - z*s;
    float z2 = z*c + x*s;
    x = x2; 
    z = z2;
}


void Vector3::rotatex(float angle)
{
 float c = (float)cos(angle);
	float s = (float)sin(angle);
	float y2 = y*c + z*s;
	float z2 = z*c - y*s;
	y = y2; z = z2;
}

void Vector3::rotatey(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	float x2 = x*c - z*s;
	float z2 = z*c + x*s;
	x = x2; z = z2;
}

void Vector3::rotatez(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	float x2 = x*c + y*s;
	float y2 = y*c - x*s;
	x = x2; y = y2;
}

void Vector3::rotate(float rx, float ry, float rz)
{
   rotatex(rx);
   rotatey(ry);
   rotatez(rz);
}

float Vector3::distance2D(const Vector3& v) const
{
	return sqrt(pow(v.x - x,2) + pow(v.z - z, 2));
}


void Vector3::print(void)
{
	printf("\n%.2f %.2f %.2f", x, y, z);
}

void Vector3::print(char *msg)
{
	printf("\n%s = %.2f %.2f %.2f", msg, x, y, z);
}

char* Vector3::toString(void)
{
   char *msg = (char*) malloc(1000);
   sprintf( msg ,"%.1f, %.1f, %.1f", x, y, z);
   return msg;
}

Vector3 Vector3::perp2D() const
{
    return Vector3(-z, 0, x);
}

float Vector3::lenghtSqr2D()
{
    return (float)sqrt(x*x + z*z);
}

void Vector3::truncate2D(float max)
{
    if( lenghtSqr2D() > max ) {
        normalize2D("");
        *this *= max;
    }
}


