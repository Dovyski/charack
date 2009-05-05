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

#ifndef __CHARACK_MAP_GENERATOR_H_
#define __CHARACK_MAP_GENERATOR_H_

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "CharackCoastGenerator.h"
#include "../util/CharackVector3.h"
#include "../util/perlin.h"

#define BLACK 0
#define BACK 1
#define GRID 2
#define WHITE 3
#define BLUE0 4

#define MAXCOL	10
typedef int CTable[MAXCOL][3];
    
#ifndef PI
	#define PI 3.14159265358979
#endif

#ifndef DEG2RAD
	#define DEG2RAD 0.0174532918661 /* pi/180 */
#endif

/**
 * Generates the map that all Charack classes will use to create the virtual world. The generated map
 * is a small representation of all elements in the world, such as continents and oceans.
 *
 * This class was created because its very hard to define land/water portions in the virtual world
 * using only math functions. Using the small representation generated by CharackMapGenerator, the
 * other classes can "guess" what to create in a specific position in the world (at least they will
 * know if the specific position is a piece of land or water).
 *
 * This class uses the continent generation algorithm by Torben AE. Mogensen <torbenm@diku.dk>. The original code
 * was downloaded from http://www.diku.dk/hjemmesider/ansatte/torbenm/, I just removed the code Charack will not
 * need and encapsulated the rest all togheter inside a class.
 */
class CharackMapGenerator {
	private:
		Perlin *mPerlinNoise;

		int altColors;
		int BLUE1, LAND0, LAND1, LAND2, LAND4;
		int GREEN1, BROWN0, GREY0;
		int back;
		int nocols;
		int rtable[256], gtable[256], btable[256];
		int lighter; /* specifies lighter colours */
		double M;   /* initial altitude (slightly below sea level) */
		double dd1;   /* weight for altitude difference */
		double dd2;  /* weight for distance */
		double POW;  /* power for distance function */

		int Depth; /* depth of subdivisions */
		double r1,r2,r3,r4; /* seeds */
		double longi,lat,scale;
		double vgrid, hgrid;

		int latic; /* flag for latitude based colour */

		int mDescriptionMatrix[CK_MACRO_MATRIX_WIDTH][CK_MACRO_MATRIX_HEIGHT];
		unsigned char col[CK_MACRO_MATRIX_WIDTH][CK_MACRO_MATRIX_HEIGHT];
		int **heights;
		int cl0[60][30];

		int do_outline;
		int do_bw;
		int *outx, *outy;

		double cla, sla, clo, slo;

		double rseed;
		double increment;

		int best;
		int weight[30];

		int min_dov(int x, int y);
		int max_dov(int x, int y);
		double fmin_dov(double x, double y);
		double fmax_dov(double x, double y);
		void setcolours();
		void makeoutline(int do_bw);
		void mercator();
		int planet0(double x, double y, double z);
		double planet(double a,double b,double c,double d, double as, double bs, double cs, double ds, double ax, double ay, double az, double bx, double by, double bz, double cx, double cy, double cz, double dx, double dy, double dz, double x, double y, double z, int level);
		double planet1(double x, double y, double z);
		double rand2(double p, double q);
		void printbmp(FILE *outfile);
		void printbmpBW(FILE *outfile);
		double log_2(double x);
		
		int isWater(int theI, int theJ);
		void buildDescriptionMatrix();
		int highResolutionIsLand(float theX, float theZ);

	public:
		static const int WATER			= 0; // water
		static const int LAND			= 1; // pure land, not a coast line.
		static const int LAND_COAST		= 2; // land, but a coast line one.

		static const int RESOLUTION_LOW		= 0;
		static const int RESOLUTION_HIGH	= 1;

		static const int MOVE_RIGHT		= 1;
		static const int MOVE_LEFT		= -1;
		static const int MOVE_UP		= -1;
		static const int MOVE_DOWN		= 1;

		CharackMapGenerator();
		~CharackMapGenerator();

		// Generate the world (land and water)
		void generate();

		// Globaly check if a specific position is land or water. The test is made against the macro world information,
		// which means coast lines and similar things are not analyzed. The method isLand() will use the information
		// of globalIsLand() as a "clue" to identify the coast lines, so it can generate a highly detailed map
		// (with noised coast lines) that can be used to check if a specific position is land or water.
		int isLand(float theX, float theZ, int theResolution = CharackMapGenerator::RESOLUTION_HIGH);

		// Get information about the specified position. The possible descriptions: CharackMapGenerator::WATER (ocean), 
		// CharackMapGenerator::LAND (land within the coastline) and CharackMapGenerator::LAND_COAST (the coast line itself).
		int getDescription(float theX, float theZ);

		// Calculates the distance between two world positions.
		int distanceFrom(int theTargetType, int theResolution, float theXObserver, float theZObserver, int theSample, int theDirection, int theMaxSteps);
};

#endif