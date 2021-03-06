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

#ifdef THINK_C
#define macintosh 1
#endif

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define BLACK 0
#define BACK 1
#define GRID 2
#define WHITE 3
#define BLUE0 4

int altColors = 0;

char view;

int BLUE1, LAND0, LAND1, LAND2, LAND4;
int GREEN1, BROWN0, GREY0;

int back = BACK;

#define MAXCOL	10
typedef int CTable[MAXCOL][3];

CTable	colors =
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

CTable	alt_colors =
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
    /*
     *	This color table tries to follow the coloring conventions of
     *	several atlases.
     *
     *	The first two colors are reserved for black and white
     *	1/4 of the colors are blue for the sea, dark being deep
     *	3/4 of the colors are land, divided as follows:
     *	 nearly 1/2 of the colors are greens, with the low being dark
     *	 1/8 of the colors shade from green through brown to grey
     *	 1/8 of the colors are shades of grey for the highest altitudes
     *
     */
    
int nocols = 256;

int rtable[256], gtable[256], btable[256];

int lighter = 0; /* specifies lighter colours */

/* Character table for XPM output */

char letters[64] = {
	'@','$','.',',',':',';','-','+','=','#','*','&','A','B','C','D',
	'E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T',
	'U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j',
	'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

#define PI 3.14159265358979
#define DEG2RAD 0.0174532918661 /* pi/180 */

/* these three values can be changed to change world characteristica */

double M  = -.02;   /* initial altitude (slightly below sea level) */
double dd1 = 0.4;   /* weight for altitude difference */
double dd2 = 0.03;  /* weight for distance */
double POW = 0.47;  /* power for distance function */

int Depth; /* depth of subdivisions */
double r1,r2,r3,r4; /* seeds */
double longi,lat,scale;
double vgrid, hgrid;

int latic = 0; /* flag for latitude based colour */

int Width = 800, Height = 600;

unsigned char **col;
int **heights;
int cl0[60][30];

int do_outline = 0;
int do_bw = 0;
int *outx, *outy;

double moll_table[] = {0.0, 0.0685055811, 0.1368109534, 0.2047150027,
		       0.2720147303, 0.3385041213, 0.4039727534,
		       0.4682040106, 0.5309726991, 0.5920417499,
		       0.6511575166, 0.7080428038, 0.7623860881,
		       0.8138239166, 0.8619100185, 0.9060553621,
		       0.9453925506, 0.9783738403, 1.0};

double cla, sla, clo, slo;

double rseed, increment = 0.00000001;

int best = 500000;
int weight[30];

int min_dov(int x, int y) {
 return x < y ? x : y; 
}

int max_dov(x,y)
int x,y;
{ return(x<y ? y : x); }

double fmin_dov(x,y)
double x,y;
{ return(x<y ? x : y); }

double fmax_dov(x,y)
double x,y;
{ return(x<y ? y : x); }


int main(ac,av)
int ac;
char **av;
{
  void printbmp(), printbmpBW(), print_error();
  void mercator(), setcolours(), makeoutline();

  int i;
  double rand2(), log_2(), planet1();

  FILE *outfile, *colfile = NULL;
  char filename[256] = "p.bmp";
  int do_file = 0;

  longi = 0.0;
  lat = 0.0;
  scale = 1.0;
  rseed = 0.123;
  view = 'm';
  vgrid = hgrid = 0.0;
//  outfile = stdout;

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


	col = (unsigned char**)calloc(Width,sizeof(unsigned char*));
	if (col == 0) {
	  fprintf(stderr, "Memory allocation failed.");
	  exit(1);
	}
	for (i=0; i<Width; i++) {
	  col[i] = (unsigned char*)calloc(Height,sizeof(unsigned char));
	  if (col[i] == 0) {
	fprintf(stderr, 
		"Memory allocation failed at %d out of %d cols\n", 
		i+1,Width);
	exit(1);
	  }
	}
  
  setcolours();

  Depth = 3*((int)(log_2(scale*Height)))+6;

  r1 = rseed;

  r1 = rand2(r1,r1);
  r2 = rand2(r1,r1);
  r3 = rand2(r1,r2);
  r4 = rand2(r2,r3);

  mercator();
  makeoutline(1);

  printbmpBW(outfile);

  return(0);
}

void setcolours()
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

void makeoutline(int do_bw)
{
  int i,j,k;

  outx = (int*)calloc(Width*Height,sizeof(int));
  outy = (int*)calloc(Width*Height,sizeof(int));
  k=0;
  for (i=1; i<Width-1; i++)
    for (j=1; j<Height-1; j++)
      if ((col[i][j] >= BLUE0 && col[i][j] <= BLUE1) &&
	  (col[i-1][j] >= LAND0 || col[i+1][j] >= LAND0 ||
	   col[i][j-1] >= LAND0 || col[i][j+1] >= LAND0 ||
	   col[i-1][j-1] >= LAND0 || col[i-1][j+1] >= LAND0 ||
	   col[i+1][j-1] >= LAND0 || col[i+1][j+1] >= LAND0)) {
	outx[k] = i; outy[k++] =j;
      }
  if (do_bw)
    for (i=0; i<Width; i++)
      for (j=0; j<Height; j++) {
	if (col[i][j] >= BLUE0 && col[i][j] <= BLUE1)
	  col[i][j] = WHITE;
	else col[i][j] = BLACK;
      }
  while (k-->0) col[outx[k]][outy[k]] = BLACK;
}

void mercator()
{
  double y,scale1,cos2,theta1, log_2();
  int i,j,k, planet0();

  y = sin(lat);
  y = (1.0+y)/(1.0-y);
  y = 0.5*log(y);
  k = (int)(0.5*y*Width*scale/PI);
  for (j = 0; j < Height; j++) {
    y = PI*(2.0*(j-k)-Height)/Width/scale;
    y = exp(2.*y);
    y = (y-1.)/(y+1.);
    scale1 = scale*Width/Height/sqrt(1.0-y*y)/PI;
    cos2 = sqrt(1.0-y*y);
    Depth = 3*((int)(log_2(scale1*Height)))+3;
    for (i = 0; i < Width ; i++) {
      theta1 = longi-0.5*PI+PI*(2.0*i-Width)/Width/scale;
      col[i][j] = planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2);
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = sin(DEG2RAD*theta1);
      y = (1.0+y)/(1.0-y);
      y = 0.5*log(y);
      j = Height/2+(int)(0.5*y*Width*scale/PI)+k;
      if (j>=0 && j<Height) for (i = 0; i < Width ; i++) col[i][j] = GRID;
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta1 = 0.0; theta1>-360.0; theta1-=vgrid);
    for (theta1 = theta1; theta1<360.0; theta1+=vgrid) {
      i = (int)(0.5*Width*(1.0+scale*(DEG2RAD*theta1-longi)/PI));
      if (i>=0 && i<Width) for (j = 0; j < Height; j++) col[i][j] = GRID;
    } 
  }
}


int planet0(x,y,z)
double x,y,z;
{
  double alt, planet1();
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

double planet(a,b,c,d, as,bs,cs,ds,
	      ax,ay,az, bx,by,bz, cx,cy,cz, dx,dy,dz,
	      x,y,z, level)
double a,b,c,d;		    /* altitudes of the 4 verticess */
double as,bs,cs,ds;	    /* seeds of the 4 verticess */
double ax,ay,az, bx,by,bz,  /* vertex coordinates */
  cx,cy,cz, dx,dy,dz;
double x,y,z;		    /* goal point */
int level;		    /* levels to go */
{
  double rand2();
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

double planet1(x,y,z)
double x,y,z;
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


double rand2(p,q) /* random number generator taking two seeds */
double p,q;	  /* rand2(p,q) = rand2(q,p) is important     */
{
  double r;
  r = (p+3.14159265)*(q+3.14159265);
  return(2.*(r-(int)r)-1.);
}
 
void printbmp(outfile) /* prints picture in BMP format */
FILE *outfile;
{
  int i,j,s, W1;

  fprintf(outfile,"BM");

  W1 = (3*Width+3);
  W1 -= W1 % 4;
  s = 54+W1*Height; /* file size */
  putc(s&255,outfile);
  putc((s>>8)&255,outfile);
  putc((s>>16)&255,outfile);
  putc(s>>24,outfile);

  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(54,outfile); /* offset to data */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(40,outfile); /* size of infoheader */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);

  putc(Width&255,outfile);
  putc((Width>>8)&255,outfile);
  putc((Width>>16)&255,outfile);
  putc(Width>>24,outfile);

  putc(Height&255,outfile);
  putc((Height>>8)&255,outfile);
  putc((Height>>16)&255,outfile);
  putc(Height>>24,outfile);

  putc(1,outfile);  /* no. of planes = 1 */
  putc(0,outfile);

  putc(24,outfile);  /* bpp */
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

  putc(0,outfile); /* colours used (unspecified) */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);


  putc(0,outfile); /* important colours (all) */
  putc(0,outfile);
  putc(0,outfile);
  putc(0,outfile);


	for (j=Height-1; j>=0; j--) {
	  for (i=0; i<Width; i++) {
	putc(btable[col[i][j]],outfile);
	putc(gtable[col[i][j]],outfile);
	putc(rtable[col[i][j]],outfile);
	  }
	  for (i=3*Width; i<W1; i++) putc(0,outfile);
	}
  
  fclose(outfile);
}

void printbmpBW(outfile) /* prints picture in b/w BMP format */
FILE *outfile;
{
  int i,j,c,s, W1;

  fprintf(outfile,"BM");

  W1 = (Width+31);
  W1 -= W1 % 32;
  s = 62+(W1*Height)/8; /* file size */
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

  putc(Width&255,outfile);
  putc((Width>>8)&255,outfile);
  putc((Width>>16)&255,outfile);
  putc(Width>>24,outfile);

  putc(Height&255,outfile);
  putc((Height>>8)&255,outfile);
  putc((Height>>16)&255,outfile);
  putc(Height>>24,outfile);

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

  for (j=Height-1; j>=0; j--)
    for (i=0; i<W1; i+=8) {
      if (i<Width && col[i][j] != BLACK && col[i][j] != GRID
	  && col[i][j] != BACK)
	c=128;
      else c=0;
      if (i+1<Width && col[i+1][j] != BLACK && col[i+1][j] != GRID
	  && col[i+1][j] != BACK)
	c+=64;
      if (i+2<Width && col[i+2][j] != BLACK && col[i+2][j] != GRID
	  && col[i+2][j] != BACK)
	c+=32;
      if (i+3<Width && col[i+3][j] != BLACK && col[i+3][j] != GRID
	  && col[i+3][j] != BACK)
	c+=16;
      if (i+4<Width && col[i+4][j] != BLACK && col[i+4][j] != GRID
	  && col[i+4][j] != BACK)
	c+=8;
      if (i+5<Width && col[i+5][j] != BLACK && col[i+5][j] != GRID
	  && col[i+5][j] != BACK)
	c+=4;
      if (i+6<Width && col[i+6][j] != BLACK && col[i+6][j] != GRID
	  && col[i+6][j] != BACK)
	c+=2;
      if (i+7<Width && col[i+7][j] != BLACK && col[i+7][j] != GRID
	  && col[i+7][j] != BACK)
	c+=1;
      putc(c,outfile);
    }
  fclose(outfile);
}

char *nletters(int n, int c)
{
  int i;
  static char buffer[8];
  
  buffer[n] = '\0';

  for (i = n-1; i >= 0; i--)
  {
    buffer[i] = letters[c & 0x001F];
    c >>= 5;
  }
  
  return buffer;
}

void printxpm(outfile) /* prints picture in XPM (X-windows pixel map) format */
FILE *outfile;
{
  int x,y,i,nbytes;

  x = nocols - 1;
  for (nbytes = 0; x != 0; nbytes++)
    x >>= 5;
  
  fprintf(outfile,"/* XPM */\n");
  fprintf(outfile,"static char *xpmdata[] = {\n");
  fprintf(outfile,"/* width height ncolors chars_per_pixel */\n");
  fprintf(outfile,"\"%d %d %d %d\",\n", Width, Height, nocols, nbytes);
  fprintf(outfile,"/* colors */\n");
  for (i = 0; i < nocols; i++)
    fprintf(outfile,"\"%s c #%2.2X%2.2X%2.2X\",\n", 
	    nletters(nbytes, i), rtable[i], gtable[i], btable[i]);

  fprintf(outfile,"/* pixels */\n");
  for (y = 0 ; y < Height; y++) {
    fprintf(outfile,"\"");
    for (x = 0; x < Width; x++)
      fprintf(outfile, "%s", nletters(nbytes, col[x][y]));
    fprintf(outfile,"\",\n");
  }
  fprintf(outfile,"};\n");

  fclose(outfile);
}

void printxpmBW(outfile) /* prints picture in XPM (X-windows pixel map) format */
FILE *outfile;
{
  int x,y,nbytes;

  x = nocols - 1;
  nbytes = 1;
  
  fprintf(outfile,"/* XPM */\n");
  fprintf(outfile,"static char *xpmdata[] = {\n");
  fprintf(outfile,"/* width height ncolors chars_per_pixel */\n");
  fprintf(outfile,"\"%d %d %d %d\",\n", Width, Height, 2, nbytes);
  fprintf(outfile,"/* colors */\n");
  
  fprintf(outfile,"\". c #FFFFFF\",\n");
  fprintf(outfile,"\"X c #000000\",\n");

  fprintf(outfile,"/* pixels */\n");
  for (y = 0 ; y < Height; y++) {
    fprintf(outfile,"\"");
    for (x = 0; x < Width; x++)
      fprintf(outfile, "%s",
	      (col[x][y] == BLACK || col[x][y] == GRID || col[x][y] == BACK)
	      ? "X" : ".");
    fprintf(outfile,"\",\n");
  }
  fprintf(outfile,"};\n");

  fclose(outfile);
}
      
double log_2(x)
double x;
{ return(log(x)/log(2.0)); }

void print_error(char *filename, char *ext)
{
  fprintf(stderr,"Usage: planet [options]\n\n");
  fprintf(stderr,"options:\n");
  fprintf(stderr,"  -?                (or any illegal option) Output this text\n");
  fprintf(stderr,"  -s seed           Specifies seed as number between 0.0 and 1.0\n");
  fprintf(stderr,"  -w width          Specifies width in pixels, default = 800\n");
  fprintf(stderr,"  -h height         Specifies height in pixels, default = 600\n");
  fprintf(stderr,"  -m magnification  Specifies magnification, default = 1.0\n");
  fprintf(stderr,"  -o output_file    Specifies output file, default is %s%s\n",
                                            filename, ext);
  fprintf(stderr,"  -l longitude      Specifies longitude of centre in degrees, default = 0.0\n");
  fprintf(stderr,"  -L latitude       Specifies latitude of centre in degrees, default = 0.0\n");
  fprintf(stderr,"  -g gridsize       Specifies vertical gridsize in degrees, default = 0.0 (no grid)\n");
  fprintf(stderr,"  -G gridsize       Specifies horisontal gridsize in degrees, default = 0.0 (no grid)\n");
  fprintf(stderr,"  -i init_alt       Specifies initial altitude (default = -0.02)\n");
  fprintf(stderr,"  -c                Colour depends on latitude (default: only altitude)\n");
  fprintf(stderr,"  -C                Use lighter colours (original scheme only)\n");
  fprintf(stderr,"  -N nocols         Use nocols number of colours (default = 256, min_dovimum = 8)\n");
  fprintf(stderr,"  -a                Switch to alternative colour scheme\n");
  fprintf(stderr,"  -M file           Read colour definitions from file\n");
  fprintf(stderr,"  -O                Produce a black and white outline map\n");
  fprintf(stderr,"  -E                Trace the edges of land in black on colour map\n");
  fprintf(stderr,"  -B                Use ``bumpmap'' shading\n");
  fprintf(stderr,"  -b                Use ``bumpmap'' shading on land only\n");
  fprintf(stderr,"  -A angle          Angle of ``light'' in bumpmap shading\n");
  fprintf(stderr,"  -P                Use PPM file format (default is BMP)\n");
  fprintf(stderr,"  -x                Use XPM file format (default is BMP)\n");
  fprintf(stderr,"  -r                Reverse background colour (default: black)\n");
  fprintf(stderr,"  -V number         Distance contribution to variation (default = 0.03)\n");
  fprintf(stderr,"  -v number         Altitude contribution to variation (default = 0.4)\n");
  fprintf(stderr,"  -pprojection      Specifies projection: m = Mercator (default)\n");
  fprintf(stderr,"                                          p = Peters\n");
  fprintf(stderr,"                                          q = Square\n");
  fprintf(stderr,"                                          s = Stereographic\n");
  fprintf(stderr,"                                          o = Orthographic\n");
  fprintf(stderr,"                                          g = Gnomonic\n");
  fprintf(stderr,"                                          a = Area preserving azimuthal\n");
  fprintf(stderr,"                                          c = Conical (conformal)\n");
  fprintf(stderr,"                                          M = Mollweide\n");
  fprintf(stderr,"                                          S = Sinusoidal\n");
  fprintf(stderr,"                                          h = Heightfield\n");
  fprintf(stderr,"                                          f = Find match, see manual\n");
  exit(0);
}

/* With the -pf option a map must be given on standard input.  */
/* This map is 11 lines of 24 characters. The characters are:  */
/*    . : very strong preference for water (value=8)	       */
/*    , : strong preference for water (value=4)		       */
/*    : : preference for water (value=2)		       */
/*    ; : weak preference for water (value=1)		       */
/*    - : don't care (value=0)				       */
/*    * : weak preference for land (value=1)		       */
/*    o : preference for land (value=2)			       */
/*    O : strong preference for land (value=4)		       */
/*    @ : very strong preference for land (value=8)	       */
/*							       */
/* Each point on the map corresponds to a point on a 15� grid. */
/*							       */
/* The program tries seeds starting from the specified and     */
/* successively outputs the seed (and rotation) of the best    */
/* current match, together with a small map of this.	       */
/* This is all ascii, no bitmap is produced.		       a*/

