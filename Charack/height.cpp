#include "height.h"

Perlin gPerlinNoise(16, 6, 1, 94);

////////////////////////////////////////////////////////
// X axis functions
////////////////////////////////////////////////////////

// This is the first "layer". Due to this fact, here we need wide waves, which will
// genarate the continent. Additionality, we need high waves (to create deep areas, which
// will become the bottom of the oceans).
float fx1(float a) {
	return gPerlinNoise.Get(a/(CK_MAX_WIDTH/5), 0.22) * 800;
}

// This is the second "layer". The first layer has already defined the shape
// of our world, so this second function will generate the mountains and similar
// stuffs. Here we need a little bit more frequency and height in the waves,
float fx2(float a) {
	return 0;
}

float fx3(float a) {
	return 0;
}



////////////////////////////////////////////////////////
// Z axis functions
////////////////////////////////////////////////////////

float fz1(float a) {
	return gPerlinNoise.Get(a/(CK_MAX_WIDTH/10), 0.22) * 800;
}

float fz2(float a) {
	return 0;
}

float fz3(float a) { return cos(a/(CK_MAX_WIDTH/200000)); }