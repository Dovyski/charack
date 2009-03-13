#ifndef _DEFS_H	
#define _DEFS_H	


#define SCREEN_X  600
#define SCREEN_Y  480

#define DIM_TERRAIN      512

#define N_TRI            DIM_TERRAIN*DIM_TERRAIN*2
#define N_VERT           (DIM_TERRAIN+1)*(DIM_TERRAIN+1)
#define N_COORD          N_VERT*3
//#define N_TEXT           N_VERT*2
#define N_INDEX          N_TRI*3

#define MIN_RES   4
#define MAX_RES   9 //9 para 512, 10 para 1024, 11 para 2048

#define STEP      100              
#define LARGURA   DIM_TERRAIN*STEP


#define SUAVIZACAO  (2.0)
#define ESCALA 4

#define PI2 PI/2

extern bool  cullingB;

#endif
