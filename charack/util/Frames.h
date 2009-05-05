/**
 * Copyright (c) 2009, Fernando Bevilacqua
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Universidade Federal de Santa Maria nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Fernando Bevilacqua ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Fernando Bevilacqua BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FRAMES_H
#define _FRAMES_H	

#include <stdio.h>
#include <time.h>

#define FRAMES 30 

////////////////////////////////////////////////////////////////////////////////////////
class Frames{
	clock_t t1, t2;   
	long    cont_frames;
	float   fps, fps_old;
public:
	Frames()
	{
	    t1 = clock();
		cont_frames = 0;
		fps_old = 0;
	}

	float getFrames()
	{
		double tempo;

	    t2 = clock();
		tempo  = (double)(t2 - t1) / CLOCKS_PER_SEC;
		cont_frames++;
		//apos 6 frames, calcula o fps medio.
		if(cont_frames==FRAMES)
		{
			t1 = t2;
			fps = FRAMES/tempo;
			cont_frames=0;
			fps_old = fps; 

			return fps;
		}
		//se nao passaram 6 frames, retorna a taxa antiga
		return fps_old;
	}
};

#endif
