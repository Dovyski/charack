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

#define _CRT_SECURE_NO_WARNINGS

#ifdef THINK_C
#define macintosh 1
#endif

#ifdef macintosh
#include <console.h>
#include <unix.h>
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

int debug = 0;
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

/* Supported output file types:
    BMP - Windows Bit MaPs
    PPM - Portable Pix Maps
    XPM - X-windows Pix Maps
 */

typedef enum ftype
    {
	bmp,
	ppm,
	xpm
    }
    ftype;
    
ftype file_type = bmp;

char* file_ext(ftype file_type)
{
  switch (file_type)
  {
    case bmp:
      return (".bmp");
    case ppm:
      return (".ppm");
    case xpm:
      return (".xpm");
    default:
      return ("");
  }
}

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

int doshade = 0;
int shade;
unsigned char **shades;
double shade_angle = 150.0; /* angle of "light" on bumpmap */

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

void copyColors(CTable cTable)
{
  int x, y;
  
  for (x = 0; x< MAXCOL; x++)
    for (y = 0; y < 3; y++)
      colors[x][y] = cTable[x][y];
}

int main(ac,av)
int ac;
char **av;
{
  void printppm(), printppmBW(), printbmp(), printbmpBW(), printxpm(), printxpmBW(),
       printheights(), print_error();
  void mercator(), peter(), squarep(), mollweide(), sinusoid(), stereo(),
       orthographic(), gnomonic(), azimuth(), conical(), heightfield(), search();
  int i;
  double rand2(), log_2(), planet1();
  void opencolfile();
  void setcolours(), readmap(), makeoutline(), smoothshades();
  FILE *outfile, *colfile = NULL;
  char filename[256] = "planet-map";
  char colorsname[256] = "\0";
  int do_file = 0;


#ifdef macintosh
  _ftype = 'TEXT';
  _fcreator ='ttxt';

  ac = ccommand (&av);
  debug = 1;
  do_file = 1;
#endif

  longi = 0.0;
  lat = 0.0;
  scale = 1.0;
  rseed = 0.123;
  view = 'm';
  vgrid = hgrid = 0.0;
  outfile = stdout;
  
  for (i = 1; i<ac; i++) {
    if (av[i][0] == '-') {
      switch (av[i][1]) {
	case 'X' : debug = 1;
		   break;
	case 'V' : sscanf(av[++i],"%lf",&dd2);
		   break;
	case 'v' : sscanf(av[++i],"%lf",&dd1);
		   break;
	case 's' : sscanf(av[++i],"%lf",&rseed);
		   break;
	case 'w' : sscanf(av[++i],"%d",&Width);
		   break;
	case 'h' : sscanf(av[++i],"%d",&Height);
		   break;
	case 'm' : sscanf(av[++i],"%lf",&scale);
		   break;
	case 'o' : sscanf(av[++i],"%s",filename);
		   do_file = 1;
		   break;
	case 'x' : file_type =xpm;
		   break;
	case 'a' : copyColors(alt_colors);
		   altColors = 1;
		   break;
	case 'M' : sscanf(av[++i],"%s",colorsname);
		   break;
	case 'l' : sscanf(av[++i],"%lf",&longi);
		   break;
	case 'L' : sscanf(av[++i],"%lf",&lat);
		   break;
	case 'g' : sscanf(av[++i],"%lf",&vgrid);
		   break;
	case 'G' : sscanf(av[++i],"%lf",&hgrid);
		   break;
	case 'c' : latic = 1;
		   break;
	case 'O' : do_outline = 1;
		   do_bw = 1;
		   break;
	case 'E' : do_outline = 1;
		   break;
	case 'C' : lighter += 1;
		   break;
	case 'N' : sscanf(av[++i],"%d",&nocols);
		   if (nocols<8) nocols = 8;
		   if (nocols>256) nocols = 256;
		   break;
	case 'B' : doshade = 1;
		   break;
	case 'b' : doshade = 2;
		   break;
	case 'P' : file_type = ppm;
		   break;
	case 'A' : sscanf(av[++i],"%lf",&shade_angle);
		   break;
	case 'r' : if (back == BACK) back = WHITE; else back = BACK;
		   break;
	case 'i' : sscanf(av[++i],"%lf",&M);
		   break;
	case 'p' : if (strlen(av[i])>2) view = av[i][2];
	           else view = av[++i][0];
	           switch (view) {
		     case 'm' : 
		     case 'p' : 
		     case 'q' : 
		     case 's' :
		     case 'o' :
		     case 'g' :
		     case 'a' :
		     case 'c' :
		     case 'M' : 
		     case 'S' :
		     case 'h' :
		     case 'f' : break;
		     default: fprintf(stderr,"Unknown projection: %s\n",av[i]);
			      print_error(do_file ? filename : "standard output", 
					 !do_file ? "" : file_ext(file_type));
		   }
		   break;
	default: fprintf(stderr,"Unknown option: %s\n",av[i]);
		 print_error(do_file ? filename : "standard output", 
			    !do_file ? "" : file_ext(file_type));
      }
    }
    else {
      fprintf(stderr,"Unknown option: %s\n\n",av[i]);
      print_error(do_file ? filename : "standard output", 
		 !do_file ? "" : file_ext(file_type));
    }
  }

  if (colorsname[0] != '\0')
    opencolfile(colfile, colorsname);

  if (do_file &&'\0' != filename[0]) {
    if (strchr (filename, '.') == 0)
      strcpy(&(filename[strlen(filename)]), file_ext(file_type));

#ifdef macintosh
    switch (file_type)
    {
      case bmp:
	_ftype = 'BMPf';
	break;
      case ppm:
	_ftype = 'PPGM';
	break;
      case xpm:
	_ftype = 'TEXT';
	break;
    }
      
    _fcreator ='GKON';
#endif

    outfile = fopen(filename,"wb");

#ifdef macintosh
    _ftype = 'TEXT';
    _fcreator ='ttxt';
#endif

    if (outfile == NULL) {
      fprintf(stderr,
	      "Could not open output file %s, error code = %d\n",
	      filename, errno);
      exit(0);
    }
  }
  else
    outfile = stdout;
  
  if (longi>180) longi -= 360;
  longi = longi*DEG2RAD;
  lat = lat*DEG2RAD;

  sla = sin(lat); cla = cos(lat);
  slo = sin(longi); clo = cos(longi);

  if (view == 'f') readmap();

  if (view == 'h') {
    heights = (int**)calloc(Width,sizeof(int*));
    if (heights == 0) {
      fprintf(stderr, "Memory allocation failed.");
      exit(1);
    }
    for (i=0; i<Width; i++) {
      heights[i] = (int*)calloc(Height,sizeof(int));
      if (heights[i] == 0) {
	fprintf(stderr, 
		"Memory allocation failed at %d out of %d heights\n", 
		i+1,Width);
	exit(1);
      }
    }
  } else {
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
  }
  if (doshade>0) {
    shades = (unsigned char**)calloc(Width,sizeof(unsigned char*));
    if (shades == 0) {
      fprintf(stderr, "Memory allocation failed.");
      exit(1);
    }
    for (i=0; i<Width; i++) {
      shades[i] = (unsigned char*)calloc(Height,sizeof(unsigned char));
      if (shades[i] == 0) {
	fprintf(stderr, 
		"Memory allocation failed at %d out of %d shades\n", 
		i,Width);
	exit(1);
      }
    }
  }

  if (view == 'c') {
    if (lat == 0) view = 'm';
	/* Conical approaches mercator when lat -> 0 */
    if (abs(lat) >= PI - 0.000001) view = 's';
	/* Conical approaches stereo when lat -> +/- 90 */
  }
  
  setcolours();

  Depth = 3*((int)(log_2(scale*Height)))+6;

  r1 = rseed;

  r1 = rand2(r1,r1);
  r2 = rand2(r1,r1);
  r3 = rand2(r1,r2);
  r4 = rand2(r2,r3);

  if (debug && (view != 'f'))
    fprintf(stderr, "+----+----+----+----+----+\n");

  switch (view) {

    case 'm': /* Mercator projection */
      mercator();
      break;

    case 'p': /* Peters projection (area preserving cylindrical) */
      peter();
      break;

    case 'q': /* Square projection (equidistant latitudes) */
      squarep();
      break;

    case 'M': /* Mollweide projection (area preserving) */
      mollweide();
      break;

    case 'S': /* Sinusoid projection (area preserving) */
      sinusoid();
      break;

    case 's': /* Stereographic projection */
      stereo();
      break;

    case 'o': /* Orthographic projection */
      orthographic();
      break;

    case 'g': /* Gnomonic projection */
      gnomonic();
      break;

    case 'a': /* Area preserving azimuthal projection */
      azimuth();
      break;

    case 'c': /* Conical projection (conformal) */
      conical();
      break;

    case 'h': /* heightfield */
      heightfield();
      break;

    case 'f': /* Search */
      while (1) {
	search();
	rseed += increment;
	r1 = rseed;
	r1 = rand2(r1,r1);
	r2 = rand2(r1,r1);
	r3 = rand2(r1,r2);
	r4 = rand2(r2,r3);
      }
  }

  if (do_outline) makeoutline(do_bw);

  if (doshade>0) smoothshades();
  
  if (debug)
    fprintf(stderr, "\n");

  /* plot picture */
  switch (file_type)
  {
    case ppm:
      if (do_bw) printppmBW(outfile);
      else if (view != 'h') printppm(outfile);
      else printheights(outfile);
      break;
    case xpm:
      if (do_bw) printxpmBW(outfile);
      else if (view != 'h') printxpm(outfile);
      else printheights(outfile);
      break;
    case bmp:
      if (do_bw) printbmpBW(outfile);
      else if (view != 'h') printbmp(outfile);
      else printheights(outfile);
      break;
  }

  return(0);
}

void opencolfile(FILE *colfile, char* colorsname)
{
    int	    crow;

    if (NULL == (colfile = fopen(colorsname, "r")))
    {
	fprintf(stderr, 
		"Cannot open %s\n", 
		colorsname);
	exit(1);
    }
    
    for (crow = 0; !feof(colfile); crow++)
    {
	int	rValue, 
		gValue, 
		bValue,
		cValue = 0;
	int	result = 0;
	
	result = fscanf(colfile, "#%6X\n", &cValue);
	
	if (result == 1)
	{
	    rValue = cValue >> 16;
	    gValue = (cValue & 0x00FF00) >> 8;
	    bValue = (cValue & 0x0000FF);
	}
	else
	{
	    result = fscanf(colfile, " %d %d %d", &rValue, &gValue, &bValue);
	}
	
	if ((result > 0) && (crow < MAXCOL))
	{
	    colors[crow][0] = rValue;
	    colors[crow][1] = gValue;
	    colors[crow][2] = bValue;
	}
    }
}

void setcolours()
{
  int i;
  int	    crow;
    
  if (debug) {
    for (crow = 0; crow < MAXCOL; crow++)
      {
	fprintf(stderr,
		" %d %d %d\n",
		colors[crow][0], colors[crow][1], colors[crow][2]);
      }
    fprintf(stderr,"\n");
  }
  
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
	if (col[i][j] >= BLUE0)
	  col[i][j] = WHITE;
	else col[i][j] = BLACK;
      }
  while (k-->0) col[outx[k]][outy[k]] = BLACK;
}

void readmap()
{
  int i,j;
  double y;
  char c;

  Width = 47; Height = 21;
  for (j = 0; j < Height; j++) {
    y = 0.5*7.5*(2.0*j-Height+1);
    y = cos(DEG2RAD*y);
    weight[j] = (int)(100.0*y+0.5);
  }
  for (j = 0; j < Height; j+=2) {
    for(i = 0; i < Width ; i+=2) {
      c = getchar();
      switch (c) {
      case '.': cl0[i][j] = -8;
		break;
      case ',': cl0[i][j] = -4;
		break;
      case ':': cl0[i][j] = -2;
		break;
      case ';': cl0[i][j] = -1;
		break;
      case '-': cl0[i][j] = 0;
		break;
      case '*': cl0[i][j] = 1;
		break;
      case 'o': cl0[i][j] = 2;
		break;
      case 'O': cl0[i][j] = 4;
		break;
      case '@': cl0[i][j] = 8;
		break;
      default: printf("Wrong map symbol: %c\n",c);
      }
      if (i>0) cl0[i-1][j] = (cl0[i][j]+cl0[i-2][j])/2;
    }
    c = getchar(); if (c!='\n') printf("Wrong map format: %c\n",c);
  }
  for (j = 1; j < Height; j+=2)
    for(i = 0; i < Width ; i++)
      cl0[i][j] = (cl0[i][j-1]+cl0[i][j+1])/2;
}


void smoothshades()
{
  int i,j;

  for (i=0; i<Width-2; i++)
    for (j=0; j<Height-2; j++)
      shades[i][j] = (4*shades[i][j]+2*shades[i][j+1]
		      +2*shades[i+1][j]+shades[i+1][j+2]+4)/9;
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
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    y = PI*(2.0*(j-k)-Height)/Width/scale;
    y = exp(2.*y);
    y = (y-1.)/(y+1.);
    scale1 = scale*Width/Height/sqrt(1.0-y*y)/PI;
    cos2 = sqrt(1.0-y*y);
    Depth = 3*((int)(log_2(scale1*Height)))+3;
    for (i = 0; i < Width ; i++) {
      theta1 = longi-0.5*PI+PI*(2.0*i-Width)/Width/scale;
      col[i][j] = planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2);
      if (doshade>0) shades[i][j] = shade;
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

void peter()
{
  double y,cos2,theta1,scale1, log_2();
  int k,i,j,water,land, planet0();

  y = 2.0*sin(lat);
  k = (int)(0.5*y*Width*scale/PI);
  water = land = 0;
  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    y = 0.5*PI*(2.0*(j-k)-Height)/Width/scale;
    if (fabs(y)>1.0)
      for (i = 0; i < Width ; i++) {
	col[i][j] = back;
	if (doshade>0) shades[i][j] = 255;
      }
    else {
      cos2 = sqrt(1.0-y*y);
      if (cos2>0.0) {
	scale1 = scale*Width/Height/cos2/PI;
	Depth = 3*((int)(log_2(scale1*Height)))+3;
	for (i = 0; i < Width ; i++) {
	  theta1 = longi-0.5*PI+PI*(2.0*i-Width)/Width/scale;
	  col[i][j] = planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2);
	  if (doshade>0) shades[i][j] = shade;
	  if (col[i][j] < LAND0) water++; else land++;
	}
      }
    }
  }
  if (debug)
    fprintf(stderr,"\n");
  fprintf(stderr,"water percentage: %d\n",100*water/(water+land));
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = 2.0*sin(DEG2RAD*theta1);
      j = Height/2+(int)(0.5*y*Width*scale/PI)+k;
      if (j>=0 && j<Height) for (i = 0; i < Width ; i++) col[i][j] = GRID;
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta1 = 0.0; theta1>-360.0; theta1-=vgrid);
    for (theta1 = theta1; theta1<360.0; theta1+=vgrid) {
      i = (int)(0.5*Width*(1.0+scale*(DEG2RAD*theta1-longi)/PI));
      if (i>=0 && i<Width)
	for (j = max_dov(0,Height/2-(int)(Width*scale/PI)+k);
	     j < min_dov(Height,Height/2+(int)(Width*scale/PI)+k); j++)
	  col[i][j] = GRID;
    }
  }
}

void squarep()
{
  double y,scale1,theta1,cos2, log_2();
  int k,i,j, planet0();

  k = (int)(0.5*lat*Width*scale/PI);
  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    y = (2.0*(j-k)-Height)/Width/scale*PI;
    if (fabs(y)>=0.5*PI) for (i = 0; i < Width ; i++) {
      col[i][j] = back;
      if (doshade>0) shades[i][j] = 255;
    } else {
      cos2 = cos(y);
      if (cos2>0.0) {
	scale1 = scale*Width/Height/cos2/PI;
	Depth = 3*((int)(log_2(scale1*Height)))+3;
	for (i = 0; i < Width ; i++) {
	  theta1 = longi-0.5*PI+PI*(2.0*i-Width)/Width/scale;
	  col[i][j] = planet0(cos(theta1)*cos2,sin(y),-sin(theta1)*cos2);
	  if (doshade>0) shades[i][j] = shade;
	}
      }
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = DEG2RAD*theta1;
      j = Height/2+(int)(0.5*y*Width*scale/PI)+k;
      if (j>=0 && j<Height) for (i = 0; i < Width ; i++) col[i][j] = GRID;
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta1 = 0.0; theta1>-360.0; theta1-=vgrid);
    for (theta1 = theta1; theta1<360.0; theta1+=vgrid) {
      i = (int)(0.5*Width*(1.0+scale*(DEG2RAD*theta1-longi)/PI));
      if (i>=0 && i<Width)
	for (j = max_dov(0,Height/2-(int)(0.25*PI*Width*scale/PI)+k);
	     j < min_dov(Height,Height/2+(int)(0.25*PI*Width*scale/PI)+k); j++)
	  col[i][j] = GRID;
    }
  }
}

void mollweide()
{
  double x,y,y1,zz,scale1,cos2,theta1,theta2, log_2();
  int i,j,i1=1,k, planet0();

  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    y1 = 2*(2.0*j-Height)/Width/scale;
    if (fabs(y1)>=1.0) for (i = 0; i < Width ; i++) {
      col[i][j] = back;
      if (doshade>0) shades[i][j] = 255;
    } else {
      zz = sqrt(1.0-y1*y1);
      y = 2.0/PI*(y1*zz+asin(y1));
      cos2 = sqrt(1.0-y*y);
      if (cos2>0.0) {
	scale1 = scale*Width/Height/cos2/PI;
	Depth = 3*((int)(log_2(scale1*Height)))+3;
	for (i = 0; i < Width ; i++) {
	  theta1 = PI/zz*(2.0*i-Width)/Width/scale;
	  if (fabs(theta1)>PI) {
	    col[i][j] = back;
	    if (doshade>0) shades[i][j] = 255;
	  } else {
	    theta1 += longi-0.5*PI;
	    col[i][j] = planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2);
	    if (doshade>0) shades[i][j] = shade;
	  }
	}
      }
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      theta2 = fabs(theta1);
      x = floor(theta2/5.0); y = theta2/5.0-x;
      y = (1.0-y)*moll_table[(int)x]+y*moll_table[(int)x+1];
      if (theta1<0.0) y = -y;
      j = Height/2+(int)(0.25*y*Width*scale);
      if (j>=0 && j< Height) {
	for (i = max_dov(0,Width/2-(int)(0.5*Width*scale*sqrt(1.0-y*y)));
	     i < min_dov(Width,Width/2+(int)(0.5*Width*scale*sqrt(1.0-y*y))); i++)
	  col[i][j] = GRID;
      }
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta1 = 0.0; theta1>-360.0; theta1-=vgrid);
    for (theta1 = theta1; theta1<360.0; theta1+=vgrid) {
      if (DEG2RAD*theta1-longi+0.5*PI>-PI &&
	  DEG2RAD*theta1-longi+0.5*PI<=PI) {
	x = 0.5*(DEG2RAD*theta1-longi+0.5*PI)*Width*scale/PI;
	j = max_dov(0,Height/2-(int)(0.25*Width*scale));
	y = 2*(2.0*j-Height)/Width/scale;
	i = Width/2+x*sqrt(1.0-y*y);
	for (; j <= min_dov(Height,Height/2+(int)(0.25*Width*scale)); j++) {
	  y1 = 2*(2.0*j-Height)/Width/scale;
	  if (fabs(y1)<=1.0) {
	    i1 = Width/2+x*sqrt(1.0-y1*y1);
	    if (i1>=0 && i1<Width) col[i1][j] = GRID;
	  }
	  if (fabs(y)<=1.0) {
	    if (i<i1) {
	      for (k=i+1; k<i1; k++)
		if (k>00 && k<Width) col[k][j] = GRID;
	    }
	    else if (i>i1) {
	      for (k=i-1; k>i1; k--)
		if (k>=0 && k<Width) col[k][j] = GRID;
	    }
	  }
	  y = y1;
	  i = i1;
	}
      }
    }
  }
}

void sinusoid()
{
  double y,theta1,theta2,cos2,l1,i1,scale1, log_2();
  int k,i,j,l,c, planet0();

  k = (int)(lat*Width*scale/PI);
  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    y = (2.0*(j-k)-Height)/Width/scale*PI;
    if (fabs(y)>=0.5*PI) for (i = 0; i < Width ; i++) {
      col[i][j] = back;
      if (doshade>0) shades[i][j] = 255;
    } else {
      cos2 = cos(y);
      if (cos2>0.0) {
	scale1 = scale*Width/Height/cos2/PI;
	Depth = 3*((int)(log_2(scale1*Height)))+3;
	for (i = 0; i<Width; i++) {
	  l = i*12/Width;
	  l1 = l*Width/12.0;
	  i1 = i-l1;
	  theta2 = longi-0.5*PI+PI*(2.0*l1-Width)/Width/scale;
	  theta1 = (PI*(2.0*i1-Width/12)/Width/scale)/cos2;
	  if (fabs(theta1)>PI/12.0) {
	    col[i][j] = back;
	    if (doshade>0) shades[i][j] = 255;
	  } else {
	    col[i][j] =
		planet0(cos(theta1+theta2)*cos2,sin(y),
			-sin(theta1+theta2)*cos2);
	    if (doshade>0) shades[i][j] = shade;
	  }
	}
      }
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = DEG2RAD*theta1;
      cos2 = cos(y);
      j = Height/2+(int)(0.5*y*Width*scale/PI)+k;
      if (j>=0 && j<Height)
	for (i = 0; i < Width ; i++) {
	  l = i*12/Width;
	  l1 = l*Width/12.0;
	  i1 = i-l1;
	  theta2 = (PI*(2.0*i1-Width/12)/Width/scale)/cos2;
	  if (fabs(theta2)<=PI/12.0) col[i][j] = GRID;
	}
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta1 = 0.0; theta1>-360.0; theta1-=vgrid);
    for (theta1 = theta1; theta1<360.0; theta1+=vgrid) {
      i = (int)(0.5*Width*(1.0+scale*(DEG2RAD*theta1-longi)/PI));
      if (i>=0 && i<Width)
	for (j = max_dov(0,Height/2-(int)(0.25*PI*Width*scale/PI)+k);
	   j < min_dov(Height,Height/2+(int)(0.25*PI*Width*scale/PI)+k); j++) {
	    y = (2.0*(j-k)-Height)/Width/scale*PI;
	    cos2 = cos(y);
	    l = i*12/Width;
	    l1 = l*Width/12.0+Width/24.0;
	    i1 = i-l1;
	    c = l1+i1*cos2;
	    if (c>=0 && c<Width) col[c][j] = GRID;
	  }
    }
  }
}

void stereo()
{
  double x,y,ymin_dov,ymax_dov,z,zz,x1,y1,z1,theta1,theta2;
  int i,j, planet0();

  ymin_dov = 2.0;
  ymax_dov = -2.0;
  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    for (i = 0; i < Width ; i++) {
      x = (2.0*i-Width)/Height/scale;
      y = (2.0*j-Height)/Height/scale;
      z = x*x+y*y;
      zz = 0.25*(4.0+z);
      x = x/zz;
      y = y/zz;
      z = (1.0-0.25*z)/zz;
      x1 = clo*x+slo*sla*y+slo*cla*z;
      y1 = cla*y-sla*z;
      z1 = -slo*x+clo*sla*y+clo*cla*z;
      if (y1 < ymin_dov) ymin_dov = y1;
      if (y1 > ymax_dov) ymax_dov = y1;

      /* for level-of-detail effect:  Depth = 3*((int)(log_2(scale*Height)/(1.0+x1*x1+y1*y1)))+6; */

      col[i][j] = planet0(x1,y1,z1);
      if (doshade>0) shades[i][j] = shade;
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = sin(DEG2RAD*theta1);
      if (ymin_dov <= y && y <= ymax_dov) {
	zz = sqrt(1-y*y);
	for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
	  x = sin(theta2)*zz;
	  z = cos(theta2)*zz;
	  x1 = clo*x+slo*z;
	  y1 = slo*sla*x+cla*y-clo*sla*z;
	  z1 = -slo*cla*x+sla*y+clo*cla*z;
	  if (fabs(z1)<1.0){
	    i = 0.5*(Height*scale*2.0*x1*(1+z1)/(1.0-z1*z1)+Width);
	    j = 0.5*(Height*scale*2.0*y1*(1+z1)/(1.0-z1*z1)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
      y = sin(theta2);
      if (ymin_dov <= y && y <= ymax_dov) {
	for (theta1 = 0.0; theta1<360.0; theta1+=vgrid) {
	  x = sin(DEG2RAD*theta1)*cos(theta2);
	  z = cos(DEG2RAD*theta1)*cos(theta2);
	  x1 = clo*x+slo*z;
	  y1 = slo*sla*x+cla*y-clo*sla*z;
	  z1 = -slo*cla*x+sla*y+clo*cla*z;
	  if (fabs(z1)<1.0){
	    i = 0.5*(Height*scale*2.0*x1*(1+z1)/(1-z1*z1)+Width);
	    j = 0.5*(Height*scale*2.0*y1*(1+z1)/(1-z1*z1)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
}

void orthographic()
{
  double x,y,z,x1,y1,z1,ymin_dov,ymax_dov,theta1,theta2,zz;
  int i,j, planet0();

  ymin_dov = 2.0;
  ymax_dov = -2.0;
  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    for (i = 0; i < Width ; i++) {
      x = (2.0*i-Width)/Height/scale;
      y = (2.0*j-Height)/Height/scale;
      if (x*x+y*y>1.0) {
	col[i][j] = back;
	if (doshade>0) shades[i][j] = 255;
      } else {
	z = sqrt(1.0-x*x-y*y);
	x1 = clo*x+slo*sla*y+slo*cla*z;
	y1 = cla*y-sla*z;
	z1 = -slo*x+clo*sla*y+clo*cla*z;
	if (y1 < ymin_dov) ymin_dov = y1;
	if (y1 > ymax_dov) ymax_dov = y1;
	col[i][j] = planet0(x1,y1,z1);
	if (doshade>0) shades[i][j] = shade;
      }
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = sin(DEG2RAD*theta1);
      if (ymin_dov <= y && y <= ymax_dov) {
	zz = sqrt(1-y*y);
	for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
	  x = sin(theta2)*zz;
	  z = cos(theta2)*zz;
	  x1 = clo*x+slo*z;
	  y1 = slo*sla*x+cla*y-clo*sla*z;
	  z1 = -slo*cla*x+sla*y+clo*cla*z;
	  if (0.0>=z1){
	    i = 0.5*(Height*scale*x1+Width);
	    j = 0.5*(Height*scale*y1+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
      y = sin(theta2);
      if (ymin_dov <= y && y <= ymax_dov) {
	for (theta1 = 0.0; theta1<360.0; theta1+=vgrid) {
	  x = sin(DEG2RAD*theta1)*cos(theta2);
	  z = cos(DEG2RAD*theta1)*cos(theta2);
	  x1 = clo*x+slo*z;
	  y1 = slo*sla*x+cla*y-clo*sla*z;
	  z1 = -slo*cla*x+sla*y+clo*cla*z;
	  if (0.0>=z1){
	    i = 0.5*(Height*scale*x1+Width);
	    j = 0.5*(Height*scale*y1+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
}

void gnomonic()
{
  double x,y,z,x1,y1,z1,zz,theta1,theta2,ymin_dov,ymax_dov;
  int i,j, planet0();

  ymin_dov = 2.0;
  ymax_dov = -2.0;
  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    for (i = 0; i < Width ; i++) {
      x = (2.0*i-Width)/Height/scale;
      y = (2.0*j-Height)/Height/scale;
      zz = sqrt(1.0/(1.0+x*x+y*y));
      x = x*zz;
      y = y*zz;
      z = sqrt(1.0-x*x-y*y);
      x1 = clo*x+slo*sla*y+slo*cla*z;
      y1 = cla*y-sla*z;
      z1 = -slo*x+clo*sla*y+clo*cla*z;
      if (y1 < ymin_dov) ymin_dov = y1;
      if (y1 > ymax_dov) ymax_dov = y1;
      col[i][j] = planet0(x1,y1,z1);
      if (doshade>0) shades[i][j] = shade;
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = sin(DEG2RAD*theta1);
      if (ymin_dov <= y && y <= ymax_dov) {
	zz = sqrt(1-y*y);
	for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
	  x = sin(theta2)*zz;
	  z = cos(theta2)*zz;
	  x1 = clo*x-slo*z;
	  y1 = slo*sla*x+cla*y+clo*sla*z;
	  z1 = slo*cla*x-sla*y+clo*cla*z;
	  if (z1!=0.0){
	    i = 0.5*(Height*scale*x1/z1+Width);
	    j = 0.5*(Height*scale*y1/z1+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
      y = sin(theta2);
      if (ymin_dov <= y && y <= ymax_dov) {
	for (theta1 = 0.0; theta1<360.0; theta1+=vgrid) {
	  x = sin(DEG2RAD*theta1)*cos(theta2);
	  z = cos(DEG2RAD*theta1)*cos(theta2);
	  x1 = clo*x-slo*z;
	  y1 = slo*sla*x+cla*y+clo*sla*z;
	  z1 = slo*cla*x-sla*y+clo*cla*z;
	  if (z1!=0.0){
	    i = 0.5*(Height*scale*x1/z1+Width);
	    j = 0.5*(Height*scale*y1/z1+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
}

void azimuth()
{
  double x,y,z,x1,y1,z1,zz,theta1,theta2,ymin_dov,ymax_dov;
  int i,j, planet0();

  ymin_dov = 2.0;
  ymax_dov = -2.0;
  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    for (i = 0; i < Width ; i++) {
      x = (2.0*i-Width)/Height/scale;
      y = (2.0*j-Height)/Height/scale;
      zz = x*x+y*y;
      z = 1.0-0.5*zz;
      if (z<-1.0) {
	col[i][j] = back;
	if (doshade>0) shades[i][j] = 255;
      } else {
	zz = sqrt(1.0-0.25*zz);
	x = x*zz;
	y = y*zz;
	x1 = clo*x+slo*sla*y+slo*cla*z;
	y1 = cla*y-sla*z;
	z1 = -slo*x+clo*sla*y+clo*cla*z;
	if (y1 < ymin_dov) ymin_dov = y1;
	if (y1 > ymax_dov) ymax_dov = y1;
	col[i][j] = planet0(x1,y1,z1);
	if (doshade>0) shades[i][j] = shade;
      }
    }
  }
  if (hgrid != 0.0) { /* draw horisontal gridlines */
    for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
    for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
      y = sin(DEG2RAD*theta1);
      if (ymin_dov <= y && y <= ymax_dov) {
	zz = sqrt(1-y*y);
	for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
	  x = sin(theta2)*zz;
	  z = cos(theta2)*zz;
	  x1 = clo*x-slo*z;
	  y1 = slo*sla*x+cla*y+clo*sla*z;
	  z1 = slo*cla*x-sla*y+clo*cla*z;
	  if (z1!=-1.0){
	    i = 0.5*(Height*scale*x1/sqrt(0.5+0.5*z1)+Width);
	    j = 0.5*(Height*scale*y1/sqrt(0.5+0.5*z1)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
  if (vgrid != 0.0) { /* draw vertical gridlines */
    for (theta2=-PI; theta2<PI; theta2+=0.5/Width/scale) {
      y = sin(theta2);
      if (ymin_dov <= y && y <= ymax_dov) {
	for (theta1 = 0.0; theta1<360.0; theta1+=vgrid) {
	  x = sin(DEG2RAD*theta1)*cos(theta2);
	  z = cos(DEG2RAD*theta1)*cos(theta2);
	  x1 = clo*x-slo*z;
	  y1 = slo*sla*x+cla*y+clo*sla*z;
	  z1 = slo*cla*x-sla*y+clo*cla*z;
	  if (z1!=-1.0){
	    i = 0.5*(Height*scale*x1/sqrt(0.5+0.5*z1)+Width);
	    j = 0.5*(Height*scale*y1/sqrt(0.5+0.5*z1)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
}

void conical()
{
  double k1,c,y2,x,y,zz,x1,y1,z1,theta1,theta2,ymin_dov,ymax_dov,cos2;
  int i,j, planet0();

  ymin_dov = 2.0;
  ymax_dov = -2.0;
  if (lat>0) {
    k1 = 1.0/sin(lat);
    c = k1*k1;
    y2 = sqrt(c*(1.0-sin(lat/k1))/(1.0+sin(lat/k1)));
    for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
      for (i = 0; i < Width ; i++) {
	x = (2.0*i-Width)/Height/scale;
	y = (2.0*j-Height)/Height/scale+y2;
	zz = x*x+y*y;
	if (zz==0.0) theta1 = 0.0; else theta1 = k1*atan2(x,y);
	if (theta1<-PI || theta1>PI) {
	  col[i][j] = back;
	  if (doshade>0) shades[i][j] = 255;
	} else {
	  theta1 += longi-0.5*PI; /* theta1 is longitude */
	  theta2 = k1*asin((zz-c)/(zz+c));
	  /* theta2 is latitude */
	  if (theta2 > 0.5*PI || theta2 < -0.5*PI) {
	    col[i][j] = back;
	    if (doshade>0) shades[i][j] = 255;
	  } else {
	    cos2 = cos(theta2);
	    y = sin(theta2);
	    if (y < ymin_dov) ymin_dov = y;
	    if (y > ymax_dov) ymax_dov = y;
	    col[i][j] = planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2);
	    if (doshade>0) shades[i][j] = shade;
	  }
	}
      }
    }
    if (hgrid != 0.0) { /* draw horisontal gridlines */
      for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
      for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
	y = sin(DEG2RAD*theta1);
	if (ymin_dov <= y && y <= ymax_dov) {
	  zz = sqrt(c*(1.0+sin(DEG2RAD*theta1/k1))
		    /(1.0-sin(DEG2RAD*theta1/k1)));
	  for (theta2=-PI+longi; theta2<PI+longi; theta2+=0.5/Width/scale) {
	    z1 = theta2-longi;
	    x1 = zz*sin(z1/k1);
	    y1 = zz*cos(z1/k1);
	    i = 0.5*(Height*scale*x1+Width);
	    j = 0.5*(Height*scale*(y1-y2)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
    if (vgrid != 0.0) { /* draw vertical gridlines */
      for (theta1=-0.5*PI; theta1<0.5*PI; theta1+=0.5/Width/scale) {
	y = sin(theta1);
	if (ymin_dov <= y && y <= ymax_dov) {
	  zz = sqrt(c*(1.0+sin(theta1/k1))
		    /(1.0-sin(theta1/k1)));
	  for (theta2 = 0.0; theta2>-180.0+longi/DEG2RAD; theta2-=vgrid);
	  for (theta2 = theta2; theta2<180.0+longi/DEG2RAD; theta2+=vgrid) {
	    z1 = DEG2RAD*theta2-longi;
	    x1 = zz*sin(z1/k1);
	    y1 = zz*cos(z1/k1);
	    i = 0.5*(Height*scale*x1+Width);
	    j = 0.5*(Height*scale*(y1-y2)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
  else {
    k1 = 1.0/sin(lat);
    c = k1*k1;
    y2 = sqrt(c*(1.0-sin(lat/k1))/(1.0+sin(lat/k1)));
    for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
      for (i = 0; i < Width ; i++) {
	x = (2.0*i-Width)/Height/scale;
	y = (2.0*j-Height)/Height/scale-y2;
	zz = x*x+y*y;
	if (zz==0.0) theta1 = 0.0; else theta1 = -k1*atan2(x,-y);
	if (theta1<-PI || theta1>PI) {
	  col[i][j] = back;
	  if (doshade>0) shades[i][j] = 255;
	} else {
	  theta1 += longi-0.5*PI; /* theta1 is longitude */
	  theta2 = k1*asin((zz-c)/(zz+c));
	  /* theta2 is latitude */
	  if (theta2 > 0.5*PI || theta2 < -0.5*PI) {
	    col[i][j] = back;
	    if (doshade>0) shades[i][j] = 255;
	  } else {
	    cos2 = cos(theta2);
	    y = sin(theta2);
	    if (y < ymin_dov) ymin_dov = y;
	    if (y > ymax_dov) ymax_dov = y;
	    col[i][j] = planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2);
	    if (doshade>0) shades[i][j] = shade;
	  }
	}
      }
    }
    if (hgrid != 0.0) { /* draw horisontal gridlines */
      for (theta1 = 0.0; theta1>-90.0; theta1-=hgrid);
      for (theta1 = theta1; theta1<90.0; theta1+=hgrid) {
	y = sin(DEG2RAD*theta1);
	if (ymin_dov <= y && y <= ymax_dov) {
	  zz = sqrt(c*(1.0+sin(DEG2RAD*theta1/k1))
		    /(1.0-sin(DEG2RAD*theta1/k1)));
	  for (theta2=-PI+longi; theta2<PI+longi; theta2+=0.5/Width/scale) {
	    z1 = theta2-longi;
	    x1 = -zz*sin(z1/k1);
	    y1 = -zz*cos(z1/k1);
	    i = 0.5*(Height*scale*x1+Width);
	    j = 0.5*(Height*scale*(y1+y2)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
    if (vgrid != 0.0) { /* draw vertical gridlines */
      for (theta1=-0.5*PI; theta1<0.5*PI; theta1+=0.5/Width/scale) {
	y = sin(theta1);
	if (ymin_dov <= y && y <= ymax_dov) {
	  zz = sqrt(c*(1.0+sin(theta1/k1))
		    /(1.0-sin(theta1/k1)));
	  for (theta2 = 0.0; theta2>-180.0+longi/DEG2RAD; theta2-=vgrid);
	  for (theta2 = theta2; theta2<180.0+longi/DEG2RAD; theta2+=vgrid) {
	    z1 = DEG2RAD*theta2-longi;
	    x1 = -zz*sin(z1/k1);
	    y1 = -zz*cos(z1/k1);
	    i = 0.5*(Height*scale*x1+Width);
	    j = 0.5*(Height*scale*(y1+y2)+Height);
	    if (0<=i && i<Width && 0<=j && j<Height) col[i][j] = GRID;
	  }
	}
      }
    }
  }
}


void heightfield()
{
  double x,y,z,x1,y1,z1, planet1();
  int i,j;

  for (j = 0; j < Height; j++) {
    if (debug && ((j % (Height/25)) == 0)) {fprintf (stderr, "%c", view); fflush(stderr);}
    for (i = 0; i < Width ; i++) {
      x = (2.0*i-Width)/Height/scale;
      y = (2.0*j-Height)/Height/scale;
      if (x*x+y*y>1.0) heights[i][j] = 0;
      else {
	z = sqrt(1.0-x*x-y*y);
	x1 = clo*x+slo*sla*y+slo*cla*z;
	y1 = cla*y-sla*z;
	z1 = -slo*x+clo*sla*y+clo*cla*z;
	heights[i][j] = 10000000*planet1(x1,y1,z1);
      }
    }
  }
}


void search()
{
  double y,cos2,theta1,scale1, planet1(), log_2();
  double y2,cos22,theta12;
  int i,j,k,l,c,c1,c2,c3, errcount, errcount1, planet0();

  for (j = 0; j < Height; j++) {
    y = 0.5*7.5*(2.0*j-Height+1);
    y = sin(DEG2RAD*y);
    scale1 = Width/Height/sqrt(1.0-y*y)/PI;
    cos2 = sqrt(1.0-y*y);
    y2 = 0.5*7.5*(2.0*j-Height+1.5);
    y2 = sin(DEG2RAD*y2);
    cos22 = sqrt(1.0-y2*y2);
    Depth = 3*((int)(log_2(scale1*Height)))+6;
    for (i = 0; i < Width ; i++) {
      theta1 = -0.5*PI+PI*(2.0*i-Width)/Width;
      theta12 = -0.5*PI+PI*(2.0*i+0.5-Width)/Width;
      c = 128+1000*planet1(cos(theta1)*cos2,y,-sin(theta1)*cos2);
      c1 = 128+1000*planet1(cos(theta12)*cos2,y,-sin(theta12)*cos2);
      c2 = 128+1000*planet1(cos(theta1)*cos22,y2,-sin(theta1)*cos22);
      c3 = 128+1000*planet1(cos(theta12)*cos22,y2,-sin(theta12)*cos22);
      c = (c+c1+c2+c3)/4.0;
      if (c<0) c = 0;
      if (c>255) c = 255;
      col[i][j] = c;
    }
  }
  for (k=0; k<Width; k++) {
    for (l=-20; l<=20; l+=2) {
      errcount = 0;
      for (j = 0; j < Height; j++) {
	errcount1 = 0;
	for(i = 0; i < Width ; i++) {
	  if (cl0[i][j]<0 && col[(i+k)%Width][j] > 128-l)
	    errcount1-=cl0[i][j];
	  if (cl0[i][j]>0 && col[(i+k)%Width][j] <= 128-l)
	    errcount1+=cl0[i][j];
	}
	errcount += weight[j]*errcount1;
      }

      if (errcount < best) {
	printf("Errors: %d, parameters: -s %.12f -l %.1f -i %.3f\n",
	       errcount,rseed,(360.0*k)/(Width+1),M+l/1000.0);
	best = errcount;
	for (j = 0; j < Height; j++) {
	  for(i = 0; i < Width ; i++)
	    if (col[(i+k)%Width][j] <= 128-l) putchar('.');
	    else putchar('O');
	  putchar('\n');
	}
	fflush(stdout);
      }
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
  double x1,y1,z1,x2,y2,z2,l1,tmp;

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
    if (doshade>0) {
      x1 = 0.25*(ax+bx+cx+dx);
      x1 = a*(x1-ax)+b*(x1-bx)+c*(x1-cx)+d*(x1-dx);
      y1 = 0.25*(ay+by+cy+dy);
      y1 = a*(y1-ay)+b*(y1-by)+c*(y1-cy)+d*(y1-dy);
      z1 = 0.25*(az+bz+cz+dz);
      z1 = a*(z1-az)+b*(z1-bz)+c*(z1-cz)+d*(z1-dz);
    l1 = sqrt(x1*x1+y1*y1+z1*z1);
      if (l1==0.0) l1 = 1.0;
      tmp = sqrt(1.0-y*y);
      if (tmp<0.0001) tmp = 0.0001;
      x2 = x*x1+y*y1+z*z1;
      y2 = -x*y/tmp*x1+tmp*y1-z*y/tmp*z1;
      z2 = -z/tmp*x1+x/tmp*z1;
      shade =
	(int)((-sin(PI*shade_angle/180.0)*y2-cos(PI*shade_angle/180.0)*z2)
	      /l1*48.0+128.0);
    if (shade<10) shade = 10;
    if (shade>255) shade = 255;
    if (doshade==2 && (a+b+c+d)<0.0) shade = 150;
  }
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

void printppm(outfile) /* prints picture in PPM (portable pixel map) format */
FILE *outfile;
{
  int i,j,c,s;

  fprintf(outfile,"P6\n");
  fprintf(outfile,"#fractal planet image\n");
  fprintf(outfile,"%d %d 255\n",Width,Height);
 
  if (doshade) {
    for (j=0; j<Height; j++) {
      for (i=0; i<Width; i++) {
	s =shades[i][j];
	c = s*rtable[col[i][j]]/150;
	if (c>255) c=255;
	putc(c,outfile);
	c = s*gtable[col[i][j]]/150;
	if (c>255) c=255;
	putc(c,outfile);
	c = s*btable[col[i][j]]/150;
	if (c>255) c=255;
	putc(c,outfile);
      }
    }
  } else {
    for (j=0; j<Height; j++)
      for (i=0; i<Width; i++) {
	putc(rtable[col[i][j]],outfile);
	putc(gtable[col[i][j]],outfile);
	putc(btable[col[i][j]],outfile);
      }
  }
  fclose(outfile);
}

void printppmBW(outfile) /* prints picture in b/w PPM format */
FILE *outfile;
{
  int i,j,c;

  fprintf(outfile,"P6\n");
  fprintf(outfile,"#fractal planet image\n");
  fprintf(outfile,"%d %d 1\n",Width,Height);
 
  for (j=0; j<Height; j++)
    for (i=0; i<Width; i++) {
      if (col[i][j] == BLACK || col[i][j] == GRID || col[i][j] == BACK)
	c=0;
      else c=1;
      putc(c,outfile);
      putc(c,outfile);
      putc(c,outfile);
    }
  fclose(outfile);
}
 
void printbmp(outfile) /* prints picture in BMP format */
FILE *outfile;
{
  int i,j,c,s, W1;

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

  if (doshade) {
    for (j=Height-1; j>=0; j--) {
      for (i=0; i<Width; i++) {
	s =shades[i][j];
	c = s*btable[col[i][j]]/150;
	if (c>255) c=255;
	putc(c,outfile);
	c = s*gtable[col[i][j]]/150;
	if (c>255) c=255;
	putc(c,outfile);
	c = s*rtable[col[i][j]]/150;
	if (c>255) c=255;
	putc(c,outfile);
      }
      for (i=3*Width; i<W1; i++) putc(0,outfile);
    }
  } else {
    for (j=Height-1; j>=0; j--) {
      for (i=0; i<Width; i++) {
	putc(btable[col[i][j]],outfile);
	putc(gtable[col[i][j]],outfile);
	putc(rtable[col[i][j]],outfile);
      }
      for (i=3*Width; i<W1; i++) putc(0,outfile);
    }
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

void printheights(outfile) /* prints heightfield */
FILE *outfile;
{
  int i,j;

  for (j=0; j<Height; j++) {
    for (i=0; i<Width; i++)
      fprintf(outfile,"%d ",heights[i][j]);
    putc('\n',outfile);
  }
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
/* Each point on the map corresponds to a point on a 15° grid. */
/*							       */
/* The program tries seeds starting from the specified and     */
/* successively outputs the seed (and rotation) of the best    */
/* current match, together with a small map of this.	       */
/* This is all ascii, no bitmap is produced.		       a*/

