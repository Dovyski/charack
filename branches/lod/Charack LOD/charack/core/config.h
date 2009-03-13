#ifndef __CHARACK_CONFIG_H_
#define __CHARACK_CONFIG_H_

// How far the user can see, in pixels
#define CK_VIEW_FRUSTUM					1000

#define CK_SCALE						5

#define CK_SEA_LEVEL					400
#define CK_SEA_BOTTON					0

#define CK_COAST_MAX_STEP				100
#define CK_COAST_MAX_SEA_DISTANCE		(CK_COAST_MAX_STEP * 4)
#define CK_COAST_VARIATION				0.20
#define CK_COAST_BEACH_HEIGHT			100

// Max world width/height
#define CK_MAX_WIDTH					3000000.0

// Useful macros
#define CK_DEG2RAD(X)					((PI*(X))/180)

#endif