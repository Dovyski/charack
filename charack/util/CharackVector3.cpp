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

#include "CharackVector3.h"


//----------------------------------------------------------------------
// Construction/Destruction
//----------------------------------------------------------------------

CharackVector3::CharackVector3() 
{ 
   x = y = z = a = 0;    
}

CharackVector3::CharackVector3(float vx, float vy, float vz) 
{
   x = vx;  y = vy;  z = vz; 
}

CharackVector3::CharackVector3(float vx, float vy, float vz,  float va) 
{
   x = vx;  y = vy;  z = vz;  a = va;
}

CharackVector3::CharackVector3(const CharackVector3& u)
{
	//printf("\ncopia");
   x = u.x; 
	y = u.y; 
	z = u.z;
}

CharackVector3::~CharackVector3()
{

}

void CharackVector3::set( const float vx, const float vy, const float vz )
{
	x = vx;
	y = vy;
	z = vz;						
};

void CharackVector3::set(const CharackVector3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;						
};


//----------------------------------------------------------------------
// OPERATORS OPERATORS OPERATORS OPERATORS
//----------------------------------------------------------------------
CharackVector3& CharackVector3::operator-= (const CharackVector3& v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

CharackVector3& CharackVector3::operator+= (const CharackVector3& v)
{
	x += v.x; y += v.y; z += v.z;
	return *this;
}


CharackVector3& CharackVector3::operator*= (float f)
{
	x *= f; y *= f; z *= f;
	return *this;
}

CharackVector3 CharackVector3::operator + ( const CharackVector3& v )
{
	CharackVector3 aux( x + v.x, y + v.y, z + v.z );
	return( aux );
}

CharackVector3 CharackVector3::operator / (float f)
{
	CharackVector3 aux( x/f, y/f, z/f );
	return( aux );
}

//	Subtracao de vetores
CharackVector3 CharackVector3::operator - ( const CharackVector3& v )
{
	CharackVector3 aux( x - v.x, y - v.y, z - v.z );
	return( aux );
}

// cross
CharackVector3 CharackVector3::operator ^ (const CharackVector3& v)
{
	CharackVector3 aux( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
	return( aux );
}

//	Produto por escalar (float)
CharackVector3 CharackVector3::operator * ( const float s )
{
	CharackVector3 aux( x * s, y * s, z * s );
	return( aux );
}

float CharackVector3::dot(const CharackVector3 &v)
{
    return x*v.x + y*v.y + z*v.z;
}

float CharackVector3::dot2D(const CharackVector3 &v)
{
    return x*v.x + z*v.z;
}

float CharackVector3::angle2D(const CharackVector3 &v)
{
    float dp, angPI ;
    
    dp = dot2D(v); //dot product
    if(dp >= 1.0) dp = 1.0f;
    if(dp <=-1.0) dp =-1.0f;
    
    angPI = (float)acos(dp);
    
    //determina a posicao relativa do vetor
    return angPI * side(v);
}

int CharackVector3::side(const CharackVector3 &v)
{
    if (z*v.x > x*v.z)
        return -1;
    else
        return 1;
}

void CharackVector3::normalize()
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

void CharackVector3::normalize2D(char *caller)
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

void CharackVector3::rotate2D(const float angle)
{
    float c = (float)cos(angle);
    float s = (float)sin(angle);
    float x2 = x*c - z*s;
    float z2 = z*c + x*s;
    x = x2; 
    z = z2;
}


void CharackVector3::rotatex(float angle)
{
 float c = (float)cos(angle);
	float s = (float)sin(angle);
	float y2 = y*c + z*s;
	float z2 = z*c - y*s;
	y = y2; z = z2;
}

void CharackVector3::rotatey(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	float x2 = x*c - z*s;
	float z2 = z*c + x*s;
	x = x2; z = z2;
}

void CharackVector3::rotatez(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	float x2 = x*c + y*s;
	float y2 = y*c - x*s;
	x = x2; y = y2;
}

void CharackVector3::rotate(float rx, float ry, float rz)
{
   rotatex(rx);
   rotatey(ry);
   rotatez(rz);
}

float CharackVector3::distance2D(const CharackVector3& v) const
{
	return sqrt(pow(v.x - x,2) + pow(v.z - z, 2));
}


void CharackVector3::print(void)
{
	printf("\n%.2f %.2f %.2f", x, y, z);
}

void CharackVector3::print(char *msg)
{
	printf("\n%s = %.2f %.2f %.2f", msg, x, y, z);
}

char* CharackVector3::toString(void)
{
   char *msg = (char*) malloc(1000);
   sprintf( msg ,"%.1f, %.1f, %.1f", x, y, z);
   return msg;
}

CharackVector3 CharackVector3::perp2D() const
{
    return CharackVector3(-z, 0, x);
}

float CharackVector3::lenghtSqr2D()
{
    return (float)sqrt(x*x + z*z);
}

void CharackVector3::truncate2D(float max)
{
    if( lenghtSqr2D() > max ) {
        normalize2D("");
        *this *= max;
    }
}


