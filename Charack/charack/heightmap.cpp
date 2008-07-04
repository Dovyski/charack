#include "charack.h"

extern CharackCamera gCamera; // TODO: fix this

const int MapWidth = 384;
const int MapHeight = 384;

float scale = 1; //the scale for the terrain *also new*

GLuint texture[10];

BYTE HeightMap[MapWidth][MapHeight];

void LoadHeightMap (char* Filename, int Width, int Height) {
	int smoothen;
	int mapX;
	int mapY;
	int mapZ;
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


void displayHeightMapDovyski(void) {
	int mapX, mapZ;
	float Height;
	int aXNow = (int)gCamera.getPositionX();
	int aZNow = (int)gCamera.getPositionZ();

	printf("x=%d,z=%d\n", aXNow, aZNow);

	for (mapX = abs(aXNow) - CK_VIEW_FRUSTUM; mapX < abs(aXNow) + CK_VIEW_FRUSTUM; mapX +=4){ 
		for (mapZ = abs(aZNow) - CK_VIEW_FRUSTUM; mapZ < abs(aZNow) + CK_VIEW_FRUSTUM; mapZ+=4){ 
			float Height1 = getHeight(mapX, mapZ);
			float Height2 = getHeight(mapX, mapZ+4);
			float Height3 = getHeight(mapX+4, mapZ);
			float Height4 = getHeight(mapX+4, mapZ+4);

			glPolygonMode(GL_FRONT,GL_LINE);

			glBegin(GL_TRIANGLE_STRIP);
				Height = getHeight(mapX, mapZ);
				glVertex3f(mapX*scale,Height,mapZ*scale);
				
				Height = getHeight(mapX, mapZ+4);
				glVertex3f(mapX*scale,Height,(mapZ+4)*scale);

				Height = getHeight(mapX+4, mapZ);
				glVertex3f((mapX+4)*scale,Height,mapZ*scale);

				Height = getHeight(mapX+4, mapZ+4);
				glVertex3f((mapX+4)*scale,Height,(mapZ+4)*scale);
			glEnd();
		}
	}
}

BYTE getHeight(float theX, float theZ) {
	int aXIndex = ((int)theX)%MapWidth;
	int aZIndex = ((int)theZ)%MapHeight;

	aXIndex = aXIndex < 0 ? 0 : aXIndex;
	aZIndex = aZIndex < 0 ? 0 : aZIndex;

	return HeightMap[aXIndex][aZIndex];
}