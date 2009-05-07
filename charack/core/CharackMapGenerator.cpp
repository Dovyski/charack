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

/** 
 * Below you can find the comments made by the original author of the source code.
 */

/* planet.c */
/* planet generating program */
/* Copyright 1988-2003 Torben AE. Mogensen */

/* version of November 21 2006 */

/* The program generates planet maps based on recursive spatial subdivision */
/* of a tetrahedron containing the globe. The output is a colour PPM bitmap. */

/* The colours may optionally be modified according to latitude to move the */
/* icecaps lower closer to the poles, with a corresponding change in land colours. */

/* The Mercator map at magnification 1 is scaled to fit the Width */
/* it uses the full height (it could extend infinitely) */
/* The orthographic projections are scaled so the full view would use the */
/* full Height. Areas outside the globe are coloured black. */
/* Stereographic and gnomic projections use the same scale as orthographic */
/* in the center of the picture, but distorts scale away from the center. */

/* It is assumed that pixels are square */
/* I have included procedures to print the maps as bmp or ppm */
/* bitmaps (portable pixel map) on standard output or specified files. */

/* I have tried to avoid using machine specific features, so it should */
/* be easy to port the program to any machine. Beware, though that due */
/* to different precision on different machines, the same seed numbers */
/* can yield very different planets. */
/* The primitive user interface is a result of portability concerns */

#include "CharackMapGenerator.h"

// TODO: fix these global vars
CTable colors =
		{{0,0,255},	    /* Dark blue depths		*/
		 {0,128,255},   /* Light blue shores	*/
		 {0,255,0},	    /* Light green lowlands	*/
		 {64,192,16},   /* Dark green highlands	*/
		 {64,192,16},   /* Dark green Mountains	*/
		 {128,128,32},  /* Brown stoney peaks	*/
		 {255,255,255}, /* White - peaks		*/
		 {0,0,0},	    /* Black - outlines		*/
		 {0,0,0},	    /* Black - background	*/
		 {0,0,0}};	    /* Black - gridlines	*/

CTable alt_colors =
		{{0,0,192},	    /* Dark blue depths		*/
		 {0,128,255},   /* Light blue shores	*/
		 {0,96,0},	    /* Dark green Lowlands	*/
		 {0,224,0},	    /* Light green Highlands	*/
		 {128,176,0},   /* Brown mountainsides	*/
		 {128,128,128}, /* Grey stoney peaks	*/
		 {255,255,255}, /* White - peaks		*/
		 {0,0,0},	    /* Black - outlines		*/
		 {0,0,0},	    /* Black - background	*/
		 {0,0,0}};	    /* Black - gridlines	*/

CharackMapGenerator::CharackMapGenerator() {
	mPerlinNoise	= new Perlin(16, 8, 1, 10);

	altColors = 0;
	back = BACK;
	    
	nocols = 256;
	lighter = 0; /* specifies lighter colours */

	M  = -.02;   /* initial altitude (slightly below sea level) */
	dd1 = 0.4;   /* weight for altitude difference */
	dd2 = 0.03;  /* weight for distance */
	POW = 0.47;  /* power for distance function */

	latic = 0; /* flag for latitude based colour */

	do_outline = 0;
	do_bw = 0;

	increment = 0.00000001;
	best = 500000;
}

CharackMapGenerator::~CharackMapGenerator() {
}


void CharackMapGenerator::generate() {
  FILE *outfile, *colfile = NULL;
  char filename[256] = "C:\\temp\\p.bmp";
  int do_file = 0;

  longi = 0.0;
  lat = 0.0;
  scale = 1.0;
  rseed = 0.123;
  
  vgrid = hgrid = 0.0;

	outfile = fopen(filename,"wb");

	if (outfile == NULL) {
		fprintf(stderr, "Could not open output file %s, error code = %d\n", filename, errno);
		exit(0);
	}
  
  if (longi>180) longi -= 360;
  longi = longi*DEG2RAD;
  lat = lat*DEG2RAD;

  sla = sin(lat); cla = cos(lat);
  slo = sin(longi); clo = cos(longi);
  
  setcolours();

  Depth = 3*((int)(log_2(scale*CK_MACRO_MATRIX_WIDTH)))+6;

  r1 = rseed;

  r1 = rand2(r1,r1);
  r2 = rand2(r1,r1);
  r3 = rand2(r1,r2);
  r4 = rand2(r2,r3);

  mercator();
  makeoutline(1);
  buildDescriptionMatrix();
	
  printbmpBW(outfile);
}

void CharackMapGenerator::setcolours()
{
  int i;
  
  while (lighter-->0) {
    int r, c;
    double x;
    
    for (r =	0; r < MAXCOL; r++)
      if (colors[r][0] != 0 || colors[r][1] != 0 || colors[r][2] != 0)
	for (c = 0; c < 3; c++) {
	  x = sqrt((double)colors[r][c]/256.0);
	  colors[r][c] = (int)(240.0*x+16);
	}
  }
  
  if (altColors) {
    if (nocols < 8)
      nocols = 8;
  
    LAND0 = max_dov(nocols / 4, BLUE0 + 1);
    BLUE1 = LAND0 - 1;
    GREY0 = nocols - (nocols / 8);
    GREEN1 = min_dov(LAND0 + (nocols / 2), GREY0 - 2);
    BROWN0 = (GREEN1 + GREY0) / 2;
    LAND1 = nocols - 1;

    rtable[GRID] = colors[9][0];
    gtable[GRID] = colors[9][1];
    btable[GRID] = colors[9][2];

    rtable[BACK] = colors[8][0];
    gtable[BACK] = colors[8][1];
    btable[BACK] = colors[8][2];

    rtable[BLACK] = colors[7][0];
    gtable[BLACK] = colors[7][1];
    btable[BLACK] = colors[7][2];

    rtable[WHITE] = colors[6][0];
    gtable[WHITE] = colors[6][1];
    btable[WHITE] = colors[6][2];

    rtable[BLUE0] = colors[0][0];
    gtable[BLUE0] = colors[0][1];
    btable[BLUE0] = colors[0][2];

    for (i=BLUE0+1;i<=BLUE1;i++) {
      rtable[i] = (colors[0][0]*(BLUE1-i)+colors[1][0]*(i-BLUE0))/(BLUE1-BLUE0);
      gtable[i] = (colors[0][1]*(BLUE1-i)+colors[1][1]*(i-BLUE0))/(BLUE1-BLUE0);
      btable[i] = (colors[0][2]*(BLUE1-i)+colors[1][2]*(i-BLUE0))/(BLUE1-BLUE0);
    }
    for (i=LAND0;i<GREEN1;i++) {
      rtable[i] = (colors[2][0]*(GREEN1-i)+colors[3][0]*(i-LAND0))/(GREEN1-LAND0);
      gtable[i] = (colors[2][1]*(GREEN1-i)+colors[3][1]*(i-LAND0))/(GREEN1-LAND0);
      btable[i] = (colors[2][2]*(GREEN1-i)+colors[3][2]*(i-LAND0))/(GREEN1-LAND0);
    }
    for (i=GREEN1;i<BROWN0;i++) {
      rtable[i] = (colors[3][0]*(BROWN0-i)+colors[4][0]*(i-GREEN1))/(BROWN0-GREEN1);
      gtable[i] = (colors[3][1]*(BROWN0-i)+colors[4][1]*(i-GREEN1))/(BROWN0-GREEN1);
      btable[i] = (colors[3][2]*(BROWN0-i)+colors[4][2]*(i-GREEN1))/(BROWN0-GREEN1);
    }
    for (i=BROWN0;i<GREY0;i++) {
      rtable[i] = (colors[4][0]*(GREY0-i)+colors[5][0]*(i-BROWN0))/(GREY0-BROWN0);
      gtable[i] = (colors[4][1]*(GREY0-i)+colors[5][1]*(i-BROWN0))/(GREY0-BROWN0);
      btable[i] = (colors[4][2]*(GREY0-i)+colors[5][2]*(i-BROWN0))/(GREY0-BROWN0);
    }
    for (i=GREY0;i<nocols;i++) {
      rtable[i] = (colors[5][0]*(nocols-i)+(colors[6][0]+1)*(i-GREY0))/(nocols-GREY0);
      gtable[i] = (colors[5][1]*(nocols-i)+(colors[6][1]+1)*(i-GREY0))/(nocols-GREY0);
      btable[i] = (colors[5][2]*(nocols-i)+(colors[6][2]+1)*(i-GREY0))/(nocols-GREY0);
    }
  } else {
    rtable[GRID] = colors[9][0];
    gtable[GRID] = colors[9][1];
    btable[GRID] = colors[9][2];

    rtable[BACK] = colors[8][0];
    gtable[BACK] = colors[8][1];
    btable[BACK] = colors[8][2];

    rtable[BLACK] = colors[7][0];
    gtable[BLACK] = colors[7][1];
    btable[BLACK] = colors[7][2];

    rtable[WHITE] = colors[6][0];
    gtable[WHITE] = colors[6][1];
    btable[WHITE] = colors[6][2];

    BLUE1 = (nocols-4)/2+BLUE0;
    if (BLUE1==BLUE0) {
      rtable[BLUE0] = colors[0][0];
      gtable[BLUE0] = colors[0][1];
      btable[BLUE0] = colors[0][2];
    } else
      for (i=BLUE0;i<=BLUE1;i++) {
	rtable[i] = (colors[0][0]*(BLUE1-i)+colors[1][0]*(i-BLUE0))/(BLUE1-BLUE0);
	gtable[i] = (colors[0][1]*(BLUE1-i)+colors[1][1]*(i-BLUE0))/(BLUE1-BLUE0);
	btable[i] = (colors[0][2]*(BLUE1-i)+colors[1][2]*(i-BLUE0))/(BLUE1-BLUE0);
      }
    LAND0 = BLUE1+1; LAND2 = nocols-2; LAND1 = (LAND0+LAND2+1)/2;
    for (i=LAND0;i<LAND1;i++) {
      rtable[i] = (colors[2][0]*(LAND1-i)+colors[3][0]*(i-LAND0))/(LAND1-LAND0);
      gtable[i] = (colors[2][1]*(LAND1-i)+colors[3][1]*(i-LAND0))/(LAND1-LAND0);
      btable[i] = (colors[2][2]*(LAND1-i)+colors[3][2]*(i-LAND0))/(LAND1-LAND0);
    }
    if (LAND1==LAND2) {
      rtable[LAND1] = colors[4][0];
      gtable[LAND1] = colors[4][1];
      btable[LAND1] = colors[4][2];
    } else
      for (i=LAND1;i<=LAND2;i++) {
	rtable[i] = (colors[4][0]*(LAND2-i)+colors[5][0]*(i-LAND1))/(LAND2-LAND1);
	gtable[i] = (colors[4][1]*(LAND2-i)+colors[5][1]*(i-LAND1))/(LAND2-LAND1);
	btable[i] = (colors[4][2]*(LAND2-i)+colors[5][2]*(i-LAND1))/(LAND2-LAND1);
      }
    LAND4 = nocols-1;
    rtable[LAND4] = colors[6][0];
    gtable[LAND4] = colors[6][1];
    btable[LAND4] = colors[6][2];
  }

}

void CharackMapGenerator::makeoutline(int do_bw)
{
  int i,j,k;

  outx = (int*)calloc(CK_MACRO_MATRIX_WIDTH*CK_MACRO_MATRIX_WIDTH,sizeof(int));
  outy = (int*)calloc(CK_MACRO_MATRIX_WIDTH*CK_MACRO_MATRIX_WIDTH,sizeof(int));
  k=0;
  for (i=1; i<CK_MACRO_MATRIX_WIDTH-1; i++)
    for (j=1; j<CK_MACRO_MATRIX_WIDTH-1; j++)
      if ((col[i][j] >= BLUE0 && col[i][j] <= BLUE1) &&
	  (col[i-1][j] >= LAND0 || col[i+1][j] >= LAND0 ||
	   col[i][j-1] >= LAND0 || col[i][j+1] >= LAND0 ||
	   col[i-1][j-1] >= LAND0 || col[i-1][j+1] >= LAND0 ||
	   col[i+1][j-1] >= LAND0 || col[i+1][j+1] >= LAND0)) {
	outx[k] = i; outy[k++] =j;
      }
  if (do_bw)
    for (i=0; i<CK_MACRO_MATRIX_WIDTH; i++)
      for (j=0; j<CK_MACRO_MATRIX_WIDTH; j++) {
	if (col[i][j] >= BLUE0 && col[i][j] <= BLUE1)
	  col[i][j] = WHITE;
	else col[i][j] = BLACK;
      }
  while (k-->0) col[outx[k]][outy[k]] = BLACK;
}


int CharackMapGenerator::isWater(int theI, int theJ) {
	return theI < 0 || theI >= CK_MACRO_MATRIX_WIDTH || theJ < 0 || theJ >= CK_MACRO_MATRIX_WIDTH || col[theI][theJ] != BLACK;
}

void CharackMapGenerator::mercator()
{
  double y,scale1,cos2,theta1;
  int i,j,k;

  y = sin(lat);
  y = (1.0+y)/(1.0-y);
  y = 0.5*log(y);
  k = (int)(0.5*y*CK_MACRO_MATRIX_WIDTH*scale/PI);
  for (j = 0; j < CK_MACRO_MATRIX_WIDTH; j++) {
    y = PI*(2.0*(j-k)-CK_MACRO_MATRIX_WIDTH)/CK_MACRO_MATRIX_WIDTH/scale;
    y = exp(2.*y);
    y = (y-1.)/(y+1.);
    scale1 = scale*CK_MACRO_MATRIX_WIDTH/CK_MACRO_MATRIX_WIDTH/sqrt(1.0-y*y)/PI;
    cos2 = sqrt(1.0-y*y);
    Depth = 3*((int)(log_2(scale1*CK_MACRO_MATRIX_WIDTH)))+3;
    for (i = 0; i < CK_MACRO_MATRIX_WIDTH ; i++) {
      theta1 = longi-0.5*PI+PI*(2.0*i-CK_MACRO_MATRIX_WIDTH)/CK_MACRO_MATRIX_WIDTH/scale;
      col[i][j] = planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2);
    }
  }
}


int CharackMapGenerator::planet0(double x, double y, double z)
{
  double alt;
  int colour;

  alt = planet1(x,y,z);

  if (altColors)
  {
    double snow = .125;
    double tree = snow * 0.5;
    double bare = (tree + snow) / 2.;
    
    if (latic) {
      snow -= (.13 * (y*y*y*y*y*y));
      bare -= (.12 * (y*y*y*y*y*y));
      tree -= (.11 * (y*y*y*y*y*y));
    }
    
    if (alt > 0) {		    /* Land */
      if (alt > snow) {		    /* Snow: White */
	colour = WHITE;
      } else if (alt > bare) {	    /* Snow: Grey - White */
	colour = GREY0+(int)((1+LAND1-GREY0) *
			      (alt-bare)/(snow-bare));
	if (colour > LAND1) colour = LAND1;
      } else if (alt > tree) {	    /* Bare: Brown - Grey */
	colour = GREEN1+(int)((1+GREY0-GREEN1) *
			      (alt-tree)/(bare-tree));
	if (colour > GREY0) colour = GREY0;
      } else {			    /* Green: Green - Brown */
	colour = LAND0+(int)((1+GREEN1-LAND0) *
			      (alt)/(tree));
	if (colour > GREEN1) colour = GREEN1;
      }
    } else {			    /* Sea */
      alt = alt/2;
      if (alt > snow) {		    /* Snow: White */
	colour = WHITE;
      } else if (alt > bare) {
	colour = GREY0+(int)((1+LAND1-GREY0) *
			      (alt-bare)/(snow-bare));
	if (colour > LAND1) colour = LAND1;
      } else {
	colour = BLUE1+(int)((BLUE1-BLUE0+1)*(25*alt));
	if (colour<BLUE0) colour = BLUE0;
      }
    }
  } else {
	/* calculate colour */
    if (alt <=0.) { /* if below sea level then */
      if (latic && y*y+alt >= 0.98)
	colour = LAND4;	 /* white if close to poles */
      else {
	colour = BLUE1+(int)((BLUE1-BLUE0+1)*(10*alt));	  /* blue scale otherwise */
	if (colour<BLUE0) colour = BLUE0;
      }
    }
    else {
      if (latic) alt += 0.10204*y*y;  /* altitude adjusted with latitude */
      if (alt >= 0.1) /* if high then */
	colour = LAND4;
      else {
	colour = LAND0+(int)((LAND2-LAND0+1)*(10*alt));
	      /* else green to brown scale */
	if (colour>LAND2) colour = LAND2;
      }
    }
  }
  return(colour);
}

double ssa,ssb,ssc,ssd, ssas,ssbs,sscs,ssds,
  ssax,ssay,ssaz, ssbx,ssby,ssbz, sscx,sscy,sscz, ssdx,ssdy,ssdz;

// a,b,c,d;		    /* altitudes of the 4 verticess */
// as,bs,cs,ds;	    /* seeds of the 4 verticess */
// ax,ay,az, bx,by,bz,  /* vertex coordinates */
// cx,cy,cz, dx,dy,dz;
// x,y,z;		    /* goal point */
// level;		    /* levels to go */

double CharackMapGenerator::planet(double a,double b,double c,double d, double as, double bs, double cs, double ds, double ax, double ay, double az, double bx, double by, double bz, double cx, double cy, double cz, double dx, double dy, double dz, double x, double y, double z, int level)
{
  double abx,aby,abz, acx,acy,acz, adx,ady,adz;
  double bcx,bcy,bcz, bdx,bdy,bdz, cdx,cdy,cdz;
  double lab, lac, lad, lbc, lbd, lcd;
  double ex, ey, ez, e, es, es1, es2, es3;
  double eax,eay,eaz, epx,epy,epz;
  double ecx,ecy,ecz, edx,edy,edz;

  if (level>0) {
    if (level==11) {
      ssa=a; ssb=b; ssc=c; ssd=d; ssas=as; ssbs=bs; sscs=cs; ssds=ds;
      ssax=ax; ssay=ay; ssaz=az; ssbx=bx; ssby=by; ssbz=bz;
      sscx=cx; sscy=cy; sscz=cz; ssdx=dx; ssdy=dy; ssdz=dz;
    }
    abx = ax-bx; aby = ay-by; abz = az-bz;
    acx = ax-cx; acy = ay-cy; acz = az-cz;
    lab = abx*abx+aby*aby+abz*abz;
    lac = acx*acx+acy*acy+acz*acz;

    if (lab<lac)
      return(planet(a,c,b,d, as,cs,bs,ds,
		    ax,ay,az, cx,cy,cz, bx,by,bz, dx,dy,dz,
		    x,y,z, level));
    else {
      adx = ax-dx; ady = ay-dy; adz = az-dz;
      lad = adx*adx+ady*ady+adz*adz;
      if (lab<lad)
	return(planet(a,d,b,c, as,ds,bs,cs,
		      ax,ay,az, dx,dy,dz, bx,by,bz, cx,cy,cz,
		      x,y,z, level));
      else {
	bcx = bx-cx; bcy = by-cy; bcz = bz-cz;
	lbc = bcx*bcx+bcy*bcy+bcz*bcz;
	if (lab<lbc)
	  return(planet(b,c,a,d, bs,cs,as,ds,
			bx,by,bz, cx,cy,cz, ax,ay,az, dx,dy,dz,
			x,y,z, level));
	else {
	  bdx = bx-dx; bdy = by-dy; bdz = bz-dz;
	  lbd = bdx*bdx+bdy*bdy+bdz*bdz;
	  if (lab<lbd)
	    return(planet(b,d,a,c, bs,ds,as,cs,
			  bx,by,bz, dx,dy,dz, ax,ay,az, cx,cy,cz,
			  x,y,z, level));
	  else {
	    cdx = cx-dx; cdy = cy-dy; cdz = cz-dz;
	    lcd = cdx*cdx+cdy*cdy+cdz*cdz;
	    if (lab<lcd)
	      return(planet(c,d,a,b, cs,ds,as,bs,
			    cx,cy,cz, dx,dy,dz, ax,ay,az, bx,by,bz,
			    x,y,z, level));
	    else {
	      es = rand2(as,bs);
	      es1 = rand2(es,es);
	      es2 = 0.5+0.1*rand2(es1,es1);
	      es3 = 1.0-es2;
	      if (ax==bx) { /* very unlikely to ever happen */
		ex = 0.5*ax+0.5*bx; ey = 0.5*ay+0.5*by; ez = 0.5*az+0.5*bz;
	      } else if (ax<bx) {
		ex = es2*ax+es3*bx; ey = es2*ay+es3*by; ez = es2*az+es3*bz;
	      } else {
		ex = es3*ax+es2*bx; ey = es3*ay+es2*by; ez = es3*az+es2*bz;
	      }
	      if (lab>1.0) lab = pow(lab,0.75);
	      e = 0.5*(a+b)+es*dd1*fabs(a-b)+es1*dd2*pow(lab,POW);
	      eax = ax-ex; eay = ay-ey; eaz = az-ez;
	      epx =  x-ex; epy =  y-ey; epz =  z-ez;
	      ecx = cx-ex; ecy = cy-ey; ecz = cz-ez;
	      edx = dx-ex; edy = dy-ey; edz = dz-ez;
	      if ((eax*ecy*edz+eay*ecz*edx+eaz*ecx*edy
		   -eaz*ecy*edx-eay*ecx*edz-eax*ecz*edy)*
		  (epx*ecy*edz+epy*ecz*edx+epz*ecx*edy
		   -epz*ecy*edx-epy*ecx*edz-epx*ecz*edy)>0.0)
		return(planet(c,d,a,e, cs,ds,as,es,
			      cx,cy,cz, dx,dy,dz, ax,ay,az, ex,ey,ez,
			      x,y,z, level-1));
	      else
		return(planet(c,d,b,e, cs,ds,bs,es,
			      cx,cy,cz, dx,dy,dz, bx,by,bz, ex,ey,ez,
			      x,y,z, level-1));
	    }
	  }
	}
      }
    } 
  }
  else {
    return((a+b+c+d)/4);
  }
}

double CharackMapGenerator::planet1(double x, double y, double z)
{
  double abx,aby,abz, acx,acy,acz, adx,ady,adz, apx,apy,apz;
  double bax,bay,baz, bcx,bcy,bcz, bdx,bdy,bdz, bpx,bpy,bpz;

  abx = ssbx-ssax; aby = ssby-ssay; abz = ssbz-ssaz;
  acx = sscx-ssax; acy = sscy-ssay; acz = sscz-ssaz;
  adx = ssdx-ssax; ady = ssdy-ssay; adz = ssdz-ssaz;
  apx = x-ssax; apy = y-ssay; apz = z-ssaz;
  if ((adx*aby*acz+ady*abz*acx+adz*abx*acy
       -adz*aby*acx-ady*abx*acz-adx*abz*acy)*
      (apx*aby*acz+apy*abz*acx+apz*abx*acy
       -apz*aby*acx-apy*abx*acz-apx*abz*acy)>0.0){
    /* p is on same side of abc as d */
    if ((acx*aby*adz+acy*abz*adx+acz*abx*ady
	 -acz*aby*adx-acy*abx*adz-acx*abz*ady)*
	(apx*aby*adz+apy*abz*adx+apz*abx*ady
	 -apz*aby*adx-apy*abx*adz-apx*abz*ady)>0.0){
      /* p is on same side of abd as c */
      if ((abx*ady*acz+aby*adz*acx+abz*adx*acy
	   -abz*ady*acx-aby*adx*acz-abx*adz*acy)*
	  (apx*ady*acz+apy*adz*acx+apz*adx*acy
	   -apz*ady*acx-apy*adx*acz-apx*adz*acy)>0.0){
	/* p is on same side of acd as b */
	bax = -abx; bay = -aby; baz = -abz;
	bcx = sscx-ssbx; bcy = sscy-ssby; bcz = sscz-ssbz;
	bdx = ssdx-ssbx; bdy = ssdy-ssby; bdz = ssdz-ssbz;
	bpx = x-ssbx; bpy = y-ssby; bpz = z-ssbz;
	if ((bax*bcy*bdz+bay*bcz*bdx+baz*bcx*bdy
	     -baz*bcy*bdx-bay*bcx*bdz-bax*bcz*bdy)*
	    (bpx*bcy*bdz+bpy*bcz*bdx+bpz*bcx*bdy
	     -bpz*bcy*bdx-bpy*bcx*bdz-bpx*bcz*bdy)>0.0){
	  /* p is on same side of bcd as a */
	  /* Hence, p is inside tetrahedron */
	  return(planet(ssa,ssb,ssc,ssd, ssas,ssbs,sscs,ssds,
			ssax,ssay,ssaz, ssbx,ssby,ssbz,
			sscx,sscy,sscz, ssdx,ssdy,ssdz,
			x,y,z, 11));
	}
      }
    }
  } /* otherwise */
  return(planet(M,M,M,M,
		/* initial altitude is M on all corners of tetrahedron */
		r1,r2,r3,r4,
		     /* same seed set is used in every call */
		0.0, 0.0, 3.01,
		0.0, sqrt(8.0)+.01*r1*r1, -1.02+.01*r2*r3,
		-sqrt(6.0)-.01*r3*r3, -sqrt(2.0)-.01*r4*r4, -1.02+.01*r1*r2,
		sqrt(6.0)-.01*r2*r2, -sqrt(2.0)-.01*r3*r3, -1.02+.01*r1*r3,
		/* coordinates of vertices */
		     x,y,z,
		     /* coordinates of point we want colour of */
		Depth));
		/* subdivision depth */

}


/* rand2(p,q) = rand2(q,p) is important     */
double CharackMapGenerator::rand2(double p, double q) /* random number generator taking two seeds */
{
  double r;
  r = (p+3.14159265)*(q+3.14159265);
  return(2.*(r-(int)r)-1.);
}

void CharackMapGenerator::printbmpBW(FILE *outfile) /* prints picture in b/w BMP format */
{
  int i,j,c,s, W1;

  fprintf(outfile,"BM");

  W1 = (CK_MACRO_MATRIX_WIDTH+31);
  W1 -= W1 % 32;
  s = 62+(W1*CK_MACRO_MATRIX_WIDTH)/8; /* file size */
  putc(s&255,outfile);
  putc((s>>8)&255,outfile);
  putc((s>>16)&255,outfile);
  putc(s>>24,outfile);

  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(62,outfile); /* offset to data */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(40,outfile); /* size of infoheader */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(CK_MACRO_MATRIX_WIDTH&255,outfile);
  putc((CK_MACRO_MATRIX_WIDTH>>8)&255,outfile);
  putc((CK_MACRO_MATRIX_WIDTH>>16)&255,outfile);
  putc(CK_MACRO_MATRIX_WIDTH>>24,outfile);

  putc(CK_MACRO_MATRIX_WIDTH&255,outfile);
  putc((CK_MACRO_MATRIX_WIDTH>>8)&255,outfile);
  putc((CK_MACRO_MATRIX_WIDTH>>16)&255,outfile);
  putc(CK_MACRO_MATRIX_WIDTH>>24,outfile);

  putc(1,outfile);  /* no. of planes = 1 */
  putc(0,outfile);

  putc(1,outfile);  /* bpp */
  putc(0,outfile);  

  putc(0,outfile); /* no compression */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(0,outfile); /* image size (unspecified) */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(0,outfile); /* h. pixels/m */
  putc(32,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(0,outfile); /* v. pixels/m */
  putc(32,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(2,outfile); /* colours used */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);


  putc(2,outfile); /* important colours (2) */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(0,outfile); /* colour 0 = black */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(255,outfile); /* colour 1 = white */
  putc(255,outfile);
  putc(255,outfile);
  putc(255,outfile);

  for (j=CK_MACRO_MATRIX_WIDTH-1; j>=0; j--)
    for (i=0; i<W1; i+=8) {
      if (i<CK_MACRO_MATRIX_WIDTH && col[i][j] != BLACK && col[i][j] != GRID
	  && col[i][j] != BACK)
	c=128;
      else c=0;
      if (i+1<CK_MACRO_MATRIX_WIDTH && col[i+1][j] != BLACK && col[i+1][j] != GRID
	  && col[i+1][j] != BACK)
	c+=64;
      if (i+2<CK_MACRO_MATRIX_WIDTH && col[i+2][j] != BLACK && col[i+2][j] != GRID
	  && col[i+2][j] != BACK)
	c+=32;
      if (i+3<CK_MACRO_MATRIX_WIDTH && col[i+3][j] != BLACK && col[i+3][j] != GRID
	  && col[i+3][j] != BACK)
	c+=16;
      if (i+4<CK_MACRO_MATRIX_WIDTH && col[i+4][j] != BLACK && col[i+4][j] != GRID
	  && col[i+4][j] != BACK)
	c+=8;
      if (i+5<CK_MACRO_MATRIX_WIDTH && col[i+5][j] != BLACK && col[i+5][j] != GRID
	  && col[i+5][j] != BACK)
	c+=4;
      if (i+6<CK_MACRO_MATRIX_WIDTH && col[i+6][j] != BLACK && col[i+6][j] != GRID
	  && col[i+6][j] != BACK)
	c+=2;
      if (i+7<CK_MACRO_MATRIX_WIDTH && col[i+7][j] != BLACK && col[i+7][j] != GRID
	  && col[i+7][j] != BACK)
	c+=1;
      putc(c,outfile);
    }
  fclose(outfile);
}
      
double CharackMapGenerator::log_2(double x)
{ return(log(x)/log(2.0)); }


int CharackMapGenerator::min_dov(int x, int y) {
 return x < y ? x : y; 
}

int CharackMapGenerator::max_dov(int x, int y)
{ return(x<y ? y : x); }

double CharackMapGenerator::fmin_dov(double x, double y)
{ return(x<y ? x : y); }

double fmax_dov(double x, double y)
{ return(x<y ? y : x); }


int CharackMapGenerator::highResolutionIsLand(float theX, float theZ) {
	return abs(mPerlinNoise->Get(theX/150000, theZ/150000)) > CK_COAST_HIGH_RES_ISLAND ? LAND : WATER;
}

int CharackMapGenerator::getMatrixIndex(float theWorldPos, int theAxis) {
	int aX = (int)abs(floor((theWorldPos/CK_MAX_WIDTH) * CK_MACRO_MATRIX_WIDTH));
	int aZ = (int)abs(floor((theWorldPos/CK_MAX_WIDTH) * CK_MACRO_MATRIX_HEIGHT));

	return theAxis == AXIS_X ? aX : aZ;
}

int CharackMapGenerator::isLand(float theX, float theZ, int theResolution) {
	int aRet = CharackMapGenerator::LAND;
	int aX = (int)abs(floor((theX/CK_MAX_WIDTH) * CK_MACRO_MATRIX_WIDTH));
	int aZ = (int)abs(floor((theZ/CK_MAX_WIDTH) * CK_MACRO_MATRIX_WIDTH));

	if(theX < 0 || theX >= CK_MAX_WIDTH || theZ < 0 || theZ >= CK_MAX_WIDTH) {
		aRet = 0;

	} else if(getDescription(theX, theZ) == CharackMapGenerator::LAND_COAST && theResolution == CharackMapGenerator::RESOLUTION_HIGH) {
		// We are over a pixel that represents a coast line. Lets provie a high resolution
		// information about land and water here in order to procude non-sharp cost lines.
		aRet = highResolutionIsLand(theX, theZ);

	} else {
		// We are over an ordinary pixel. Lets calculate the return using low resolution information.
		aRet = mDescriptionMatrix[aZ][aX];
	}

	return aRet;
}


void CharackMapGenerator::buildDescriptionMatrix() {
	int i,j, aHasWaterNeighbor = 0;

	for(i = 0; i < CK_MACRO_MATRIX_WIDTH; i++) {
		for(j = 0; j < CK_MACRO_MATRIX_WIDTH; j++) {
			mDescriptionMatrix[i][j]	= isWater(i, j) ? CharackMapGenerator::WATER : CharackMapGenerator::LAND;
			aHasWaterNeighbor			= isWater(i, j-1) || isWater(i-1, j) || isWater(i+1, j) || isWater(i, j+1);
			
			if(!isWater(i, j) && aHasWaterNeighbor) {
				mDescriptionMatrix[i][j] = CharackMapGenerator::LAND_COAST;
			}
		}
	}
}

int CharackMapGenerator::getDescription(float theX, float theZ) {
	if(theX < 0 || theX >= CK_MAX_WIDTH || theZ < 0 || theZ >= CK_MAX_WIDTH) {
		return 0;
	}

	int aX = (int)abs(floor((theX/CK_MAX_WIDTH) * CK_MACRO_MATRIX_WIDTH));
	int aZ = (int)abs(floor((theZ/CK_MAX_WIDTH) * CK_MACRO_MATRIX_WIDTH));

	aX = aX >= CK_MACRO_MATRIX_WIDTH	? CK_MACRO_MATRIX_WIDTH	-1 : aX;
	aZ = aZ >= CK_MACRO_MATRIX_WIDTH	? CK_MACRO_MATRIX_WIDTH -1 : aZ;

	return mDescriptionMatrix[aZ][aX];
}

int CharackMapGenerator::distanceFrom(int theTargetType, int theResolution, float theXObserver, float theZObserver, int theSample, int theDirection, int theMaxSteps) {
	int aDistance = 0, aSteps = 0;
	
	theSample  *= theDirection;

	while(aSteps < theMaxSteps) {
		if(isLand(theXObserver, theZObserver, theResolution) == theTargetType) {
			break; // target have been found
		} else {
			if(theDirection == MOVE_RIGHT || theDirection == MOVE_LEFT) {
				theXObserver += theSample;
			} else {
				theZObserver += theSample;
			}
			aDistance++;
			aSteps++;
		}
	}

	return aDistance;
}

int **CharackMapGenerator::getDescriptionMatrix() {
	return (int **)mDescriptionMatrix;
}