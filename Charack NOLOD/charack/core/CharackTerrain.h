#ifndef _CHARACKTERRAIN_H
#define _CHARACKTERRAIN_H	


#ifdef _WIN32
	#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "../util/vec3f.h"
#include "config.h"

class CharackTerrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		CharackTerrain(int w2, int l2);
		
		~CharackTerrain();
		
		int width();
		
		int length();
		
		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y);
		
		//Returns the height at (x, z)
		float getHeight(int x, int z);
		
		//Computes the normals, if they haven't been computed yet
		void computeNormals();
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z);

		void render();

		void loadData(unsigned char *theData);
};

#endif