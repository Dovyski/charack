#include "height.h"

Perlin gPerlinNoise(16, 6, 1, 94);
Perlin gMediumPerlinNoise(16, 8, 1, 94);

float fx1(float a) {
	return gMediumPerlinNoise.Get(a/(CK_MAX_WIDTH/50), 0.22) * 800;
//	return gPerlinNoise.Get(a/(CK_MAX_WIDTH/5), 0.22) * 800 + gMediumPerlinNoise.Get(a/(CK_MAX_WIDTH/50), 0.22) * 800;
}

float fz1(float a) {
	return gMediumPerlinNoise.Get(a/(CK_MAX_WIDTH/20), 0.22) * 800;
}