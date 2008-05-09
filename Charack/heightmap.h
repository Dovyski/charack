#ifndef _HEIGHT_MAP_H_
#define _HEIGHT_MAP_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

//This tutorial is going to extend on the last heightmap tutorial
//The difference here is that I am now adding lighting to the
//terrain using a thing known as a normal. I have also added a little scale
//feature to the terrain, so you can set the spacing between the vertices.

//So lets get started shall we

//--------------------------------------------------
//First Section: Variables

//Now there are a few variables that I have added, some of them
//are explained in the lighting tutorials.

//This is the scale for the terrain, we can set
//the distance between the vertices with this. This can
//be used to create larger terrains.
//float scale = 1; 

//here i am creating a struct for our normals
//typedef struct
//{
//float x; //the x position of the current normal
//float y; //the y position of the current normal
//float z; //the z position of the current normal
//}NORMALS; //I have chosen to call them NORMALS - can be whatever you want

//NORMALS ab; //normal for length AB - can be whatever you want
//NORMALS ac; //normal for length AC - can be whatever you want

//NORMALS n; //our final/main normal

//here we set the materials for the terrain
//GLfloat whiteDiffuseMaterial[] = {1.0, 1.0, 1.0}; //set the diffuse material to white
//GLfloat greyAmbientMaterial[] = {0.25, 0.25, 0.25}; //set the shadows material to grey, the lower the number, the more intense the shadows
//GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the specular material to white

//diffuse light color variables - 1,1,1 makes white
//GLfloat dlr = 1.0;
//GLfloat dlg = 1.0;
//GLfloat dlb = 1.0;

//ambient light color variables - 1,1,1 makes white
//GLfloat alr = 1.0;
//GLfloat alg = 1.0;
//GLfloat alb = 1.0;

//ligth position variables
//GLfloat lx = -40.0; the x position
//GLfloat ly = 100.0; the y position
//GLfloat lz = -30.0; the z position
//GLfloat lw = 0.0; //setting this to 0 sets the light as a diffuse light

//--------------------------------------------------
//Section 2: Calculating the normals

//here is our function to calculate the normals
//with this you supply:
//x1,y1,z1 - the points of the current vertex
//x2,y2,z2 - the points of the vertex to the right
//x3,y3,z3 - the points of the vertex to the left
//each within the current shape.
//void calculateNormals (float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {

//double d; //d will hold the distance of each vector normal

//get the length of the vector to the right
//ab.x = x2 - x1; 
//ab.y = y2 - y1;
//ab.z = z2 - z1;
			
//get the length of the vector to the left
//ac.x = x3 - x1;
//ac.y = y3 - y1;
//ac.z = z3 - z1;

//here we do our dot product
//this is something you learn about when going
//over vectors. a dot product is equal to 0, and
//is therefore perpendicular to the current vector.
//n.x = (ab.y * ac.z) - (ab.z * ac.y);
//n.y = (ab.z * ac.x) - (ab.x * ac.z);
//n.z = (ab.x * ac.y) - (ab.y * ac.x);
	
//here we get the distance of our normal vector
//d = sqrt((n.x*n.x) + (n.y*n.y) + (n.z*n.z));

//then we normalize it by dividing the current
//vector by its distance.
//n.x = n.x / d;
//n.y = n.y / d;
//n.z = n.z / d;
//}

//--------------------------------------------------
//Section 3: Displaying the Heightmap with the Normals

//To do this the first thing we have to do is add materials
//to the object as we are using lighting now and our
//color function doesn't manage lights.

//Now in here you will notice tha I call calculate
//normals for each vertex and then add a normal.

//The hardest part here is working our which
//number goes with which x,y and z. But once
//you work it out, I believe the results
//are pretty good. Not the best but
//alot better then without shading.

//void DisplayHeightMap (void) {
//float Height;
//set the materials for the heightmap
//glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuseMaterial); 
//glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT,greyAmbientMaterial); 
//glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial); 

//for (mapX = 1; mapX < MapWidth; mapX +=4){ 
//for (mapZ = 1; mapZ < MapHeight; mapZ+=4){ 
//float Height1 = HeightMap[mapX][mapZ];
//float Height2 = HeightMap[mapX][mapZ+4];
//float Height3 = HeightMap[mapX+4][mapZ];
//float Height4 = HeightMap[mapX+4][mapZ+4];

//	glBegin(GL_TRIANGLE_STRIP);
//	Height = HeightMap[mapX][mapZ];
//	glTexCoord2f(0,0); 
	//now we calculate our normals, we do this for each vertex
//	calculateNormals(mapX,Height1,mapZ,mapX+4,Height3,mapZ,mapX,Height2,mapZ+4);
	//then we add the normal
//	glNormal3f(n.x,n.y,n.z);
//	glVertex3f(mapX*scale,Height,mapZ*scale);
	
//	Height = HeightMap[mapX][mapZ+4];
//	glTexCoord2f(0,1); 
	//now we calculate our normals, we do this for each vertex
//	calculateNormals(mapX,Height2,mapZ+4,mapX,Height1,mapZ,mapX+4,Height4,mapZ+4);
	//then we add the normal
//	glNormal3f(n.x,n.y,n.z);
//	glVertex3f(mapX*scale,Height,(mapZ+4)*scale);

//	Height = HeightMap[mapX+4][mapZ];
//	glTexCoord2f(1,0); 
	//now we calculate our normals, we do this for each vertex
//	calculateNormals(mapX+4,Height3,mapZ,mapX+4,Height4,mapZ+4,mapX,Height1,mapZ);
	//then we add the normal
//	glNormal3f(n.x,n.y,n.z);
//	glVertex3f((mapX+4)*scale,Height,mapZ*scale);

//	Height = HeightMap[mapX+4][mapZ+4];
//	glTexCoord2f(1,1); 
	//now we calculate our normals, we do this for each vertex
//	calculateNormals(mapX+4,Height4,mapZ+4,mapX,Height2,mapZ+4,mapX+4,Height3,mapZ);
	//then we add the normal
//	glNormal3f(n.x,n.y,n.z);
//	glVertex3f((mapX+4)*scale,Height,(mapZ+4)*scale);
//	glEnd();
//}
//}
//}

//And if you can work that all out, you should have a little idea on how
//to use normals. if you are not sure of the math behind it. I will
//be writing a tutorial on vector math in the forum, or you
//can just google search it as normals are not just for opengl
//they are for all 3D programs.


GLuint LoadTextureRAW( const char * filename, int width, int height );

void FreeTexture( GLuint texture );

void LoadHeightMap (char* Filename, int Width, int Height);

void DisplayHeightMap (void);

void calculateNormals (float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

void displayHeightMapDovyski(void);

BYTE getHeight(float theX, float theZ);

//here i am creating a struct for our normals
typedef struct {
	float x; //the x position of the current normal
	float y; //the y position of the current normal
	float z; //the z position of the current normal
}NORMALS; //I have chosen to call them NORMALS - can be whatever you want

extern GLfloat dlr;
extern GLfloat dlg;
extern GLfloat dlb;

//ambient light color variables
extern GLfloat alr;
extern GLfloat alg;
extern GLfloat alb;

//ligth position variables
extern GLfloat lx;
extern GLfloat ly;
extern GLfloat lz;
extern GLfloat lw;


extern GLuint texture[10];

#endif //_HEIGHT_MAP_H_