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

#ifndef __CHARACK_CONFIG_H_
#define __CHARACK_CONFIG_H_

// How many points will be collected (in X and Z) to render the world
#define CK_DIM_TERRAIN					120

// How far the user can see, in pixels
#define CK_VIEW_FRUSTUM					1000

// Max world width/	height
#define CK_MAX_WIDTH					3000000.0f
#define CK_MAX_HEIGHT					255.0f
#define CK_MACRO_MATRIX_WIDTH			400
#define CK_MACRO_MATRIX_HEIGHT			400

// OpenGL scale behavior
#define CK_SCALE						2000.0f

#define CK_SEA_LEVEL					0.0f
#define CK_SEA_BOTTON					0.0f

// Distance between coast and water (moviment origin is land)
#define CK_COAST_MAX_STEP				10
#define CK_COAST_MAX_SEA_DISTANCE		(CK_COAST_MAX_STEP * 4)
#define CK_COAST_VARIATION				0.20
#define CK_COAST_BEACH_HEIGHT			20.0f
#define CK_COAST_MANGLE_HEIGHT			1
#define CK_COAST_HIGH_RES_ISLAND		0.2

// Settings for disturbing coast lines
#define CK_COAST_DISTURBE				0
#define CK_COAST_DISTURBE_RATE			0.03f
#define CK_COAST_DISTRB_MAX_STEP		8
#define CK_COAST_DISTRB_MAX_DISTANCE	(CK_COAST_DISTRB_MAX_STEP * 4)

// Distance from one vertex to another
#define CK_MESH_SPACE					2000

// How many times the terrain manager should smoothfy the collected data before displaying it.
#define CK_HOW_SMOOTH					1
#define CK_FOG_NEAR						5000.0f
#define CK_FOG_FAR						20.0f

// Useful macros
#define CK_DEG2RAD(X)					((PI*(X))/180)

#endif
