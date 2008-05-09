#include "heightmap.h"
#include "move.h"
#include "config.h"

const int MapWidth = 384;
const int MapHeight = 384;

int mapX;
int mapY;
int mapZ;

float scale = 1; //the scale for the terrain *also new*

GLuint texture[10];

BYTE HeightMap[MapWidth][MapHeight];

NORMALS ab; //normal for length AB - can be whatever you want
NORMALS ac; //normal for length AC - can be whatever you want

NORMALS n; //our final/main normal

//here we set the materials for the terrain
GLfloat whiteDiffuseMaterial[] = {1.0, 1.0, 1.0}; //set the diffuse to white
GLfloat greyAmbientMaterial[] = {0.25, 0.25, 0.25}; //set the shadows to grey, the lower the number, the more intense the shadows
GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the specular to white

//diffuse light color variables
GLfloat dlr = 1.0;
GLfloat dlg = 1.0;
GLfloat dlb = 1.0;

//ambient light color variables
GLfloat alr = 1.0;
GLfloat alg = 1.0;
GLfloat alb = 1.0;

//ligth position variables
GLfloat lx = -50.0;
GLfloat ly = 100.0;
GLfloat lz = -50.0;
GLfloat lw = 0.0; //setting this to 0 sets the light as a diffuse light


void LoadHeightMap (char* Filename, int Width, int Height) {
	int smoothen;
	FILE * File;

	File = fopen( Filename, "rb" );

	fread( HeightMap, 1, Width * Height * 3, File );
	fclose( File );

	for (smoothen = 0; smoothen < 3; smoothen++){
		for (mapX = 1; mapX < Width; mapX ++){ 
			for (mapZ = 1; mapZ < Height * 3; mapZ++){ 
				mapY = HeightMap[mapX][mapZ];
				mapY += HeightMap[mapX-1][mapZ];
				mapY += HeightMap[mapX+1][mapZ];
				mapY += HeightMap[mapX][mapZ-1];
				mapY += HeightMap[mapX][mapZ+1];
				mapY += HeightMap[mapX-1][mapZ-1];
				mapY += HeightMap[mapX-1][mapZ+1];
				mapY += HeightMap[mapX+1][mapZ-1];
				mapY += HeightMap[mapX+1][mapZ+1];
				mapY = mapY/9;
				HeightMap[mapX][mapZ] = mapY ;
			}
		}
	}
}

void DisplayHeightMap (void) {
	float Height;
	//set the materials for the heightmap
	glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuseMaterial); 
	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT,greyAmbientMaterial); 
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial); 

	for (mapX = 1; mapX < MapWidth; mapX +=4){ 
		for (mapZ = 1; mapZ < MapHeight; mapZ+=4){ 
			float Height1 = getHeight(mapX, mapZ);
			float Height2 = getHeight(mapX, mapZ+4);
			float Height3 = getHeight(mapX+4, mapZ);
			float Height4 = getHeight(mapX+4, mapZ+4);

			glBegin(GL_TRIANGLE_STRIP);
			Height = getHeight(mapX, mapZ);
			glTexCoord2f(0,0); 
			//now we calculate our normals, we do this for each vertex
			calculateNormals(mapX,Height1,mapZ,mapX+4,Height3,mapZ,mapX,Height2,mapZ+4);
			//then we add the normal
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(mapX*scale,Height,mapZ*scale);
			
			Height = getHeight(mapX, mapZ+4);
			glTexCoord2f(0,1); 
			//now we calculate our normals, we do this for each vertex
			calculateNormals(mapX,Height2,mapZ+4,mapX,Height1,mapZ,mapX+4,Height4,mapZ+4);
			//then we add the normal
			glNormal3f(n.x,n.y,n.z);
			glVertex3f(mapX*scale,Height,(mapZ+4)*scale);

			Height = getHeight(mapX+4, mapZ);
			glTexCoord2f(1,0); 
			//now we calculate our normals, we do this for each vertex
			calculateNormals(mapX+4,Height3,mapZ,mapX+4,Height4,mapZ+4,mapX,Height1,mapZ);
			//then we add the normal
			glNormal3f(n.x,n.y,n.z);
			glVertex3f((mapX+4)*scale,Height,mapZ*scale);

			Height = getHeight(mapX+4, mapZ+4);
			glTexCoord2f(1,1); 
			//now we calculate our normals, we do this for each vertex
			calculateNormals(mapX+4,Height4,mapZ+4,mapX,Height2,mapZ+4,mapX+4,Height3,mapZ);
			//then we add the normal
			glNormal3f(n.x,n.y,n.z);
			glVertex3f((mapX+4)*scale,Height,(mapZ+4)*scale);
			glEnd();
		}
	}
}

void displayHeightMapDovyski(void) {
	float Height;
	//set the materials for the heightmap
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,		whiteDiffuseMaterial); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,		greyAmbientMaterial); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,	whiteSpecularMaterial); 
	
	int aXNow = getPositionX();
	int aZNow = getPositionZ();

	printf("%d,%d (rotY: %d)\n", aXNow, aZNow, gRotY);

	for (mapX = abs(aXNow) - CK_VIEW_FRUSTUM; mapX < abs(aXNow) + CK_VIEW_FRUSTUM; mapX +=4){ 
		for (mapZ = abs(aZNow) - CK_VIEW_FRUSTUM; mapZ < abs(aZNow) + CK_VIEW_FRUSTUM; mapZ+=4){ 
			float Height1 = getHeight(mapX, mapZ);
			float Height2 = getHeight(mapX, mapZ+4);
			float Height3 = getHeight(mapX+4, mapZ);
			float Height4 = getHeight(mapX+4, mapZ+4);

			glPolygonMode(GL_FRONT,GL_LINE);

			glBegin(GL_TRIANGLE_STRIP);
				Height = getHeight(mapX, mapZ);
				glTexCoord2f(0,0); 
				//now we calculate our normals, we do this for each vertex
				calculateNormals(mapX,Height1,mapZ,mapX+4,Height3,mapZ,mapX,Height2,mapZ+4);
				//then we add the normal
				glNormal3f(n.x,n.y,n.z);
				glVertex3f(mapX*scale,Height,mapZ*scale);
				
				Height = getHeight(mapX, mapZ+4);
				glTexCoord2f(0,1); 
				//now we calculate our normals, we do this for each vertex
				calculateNormals(mapX,Height2,mapZ+4,mapX,Height1,mapZ,mapX+4,Height4,mapZ+4);
				//then we add the normal
				glNormal3f(n.x,n.y,n.z);
				glVertex3f(mapX*scale,Height,(mapZ+4)*scale);

				Height = getHeight(mapX+4, mapZ);
				glTexCoord2f(1,0); 
				//now we calculate our normals, we do this for each vertex
				calculateNormals(mapX+4,Height3,mapZ,mapX+4,Height4,mapZ+4,mapX,Height1,mapZ);
				//then we add the normal
				glNormal3f(n.x,n.y,n.z);
				glVertex3f((mapX+4)*scale,Height,mapZ*scale);

				Height = getHeight(mapX+4, mapZ+4);
				glTexCoord2f(1,1); 
				//now we calculate our normals, we do this for each vertex
				calculateNormals(mapX+4,Height4,mapZ+4,mapX,Height2,mapZ+4,mapX+4,Height3,mapZ);
				//then we add the normal
				glNormal3f(n.x,n.y,n.z);
				glVertex3f((mapX+4)*scale,Height,(mapZ+4)*scale);
			glEnd();
		}
	}
}



GLuint LoadTextureRAW( const char * filename, int width, int height )
{
  GLuint texture[1];
  unsigned char * data;
  FILE * file;

  file = fopen( filename, "rb" );
  if ( file == NULL ) return 0;

  data = (unsigned char *)malloc( width * height * 3 );

  fread( data, width * height * 3, 1, file );
  fclose( file );

  glGenTextures(1, &texture[0] );			
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

  free( data );

  return texture[0];
}

void FreeTexture( GLuint texture ) {
  glDeleteTextures( 1, &texture );
}

//here is our function to calculate the normals
//with this you supply:
//x1,y1,z1 - the points of the current vertex
//x2,y2,z2 - the points of the vertex to the right
//x3,y3,z3 - the points of the vertex to the left
//each within the current shape.
void calculateNormals (float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
	double d; //d will hold the distance of each vector normal

	//get the length of the vector to the right
	ab.x = x2 - x1; 
	ab.y = y2 - y1;
	ab.z = z2 - z1;
				
	//get the length of the vector to the left
	ac.x = x3 - x1;
	ac.y = y3 - y1;
	ac.z = z3 - z1;

	//here we do our dot product
	//this is something you learn about when going
	//over vectors. a dot product is equal to 0, and
	//is therefore perpendicular to the current vector.
	n.x = (ab.y * ac.z) - (ab.z * ac.y);
	n.y = (ab.z * ac.x) - (ab.x * ac.z);
	n.z = (ab.x * ac.y) - (ab.y * ac.x);
		
	//here we get the distance of our normal vector
	d = sqrt((n.x*n.x) + (n.y*n.y) + (n.z*n.z));

	//then we normalize it by dividing the current
	//vector by its distance.
	n.x = n.x / d;
	n.y = n.y / d;
	n.z = n.z / d;
}


BYTE getHeight(float theX, float theZ) {
	int aXIndex = ((int)theX)%MapWidth;
	int aZIndex = ((int)theZ)%MapHeight;

	aXIndex = aXIndex < 0 ? 0 : aXIndex;
	aZIndex = aZIndex < 0 ? 0 : aZIndex;

	return HeightMap[aXIndex][aZIndex];
	//printf("%f - %d\n", sin(theX/2), (int)(floor(sin(theX/2)*255)));
	//return sin(theX/2);
	
	// montanha no -280,-130 : 	return ((theX <= 360 && theX >= 300) && (theZ <= 660 && theZ >= 200)? 100 : 0);
	//return (((theX <= 360 && theX >= 300) && (theZ <= 660 && theZ >= 200) || (theX <= 2560 && theX >= 2500) && (theZ <= 660 && theZ >= 200))? 100 : 0);
}