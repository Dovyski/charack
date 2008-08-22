#include "height.h"

float fx1(float a) { return sin(a/(CK_MAX_WIDTH/20)); }
float fx2(float a) { return sin(a/(CK_MAX_WIDTH/20000)); }
float fx3(float a) { return cos(a/(CK_MAX_WIDTH/200000)); }

float fz1(float a) { return cos(a/(CK_MAX_WIDTH/20)); }
float fz2(float a) { return cos(a/(CK_MAX_WIDTH/20000)); }
float fz3(float a) { return cos(a/(CK_MAX_WIDTH/200000)); }