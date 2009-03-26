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
#define CK_DIM_TERRAIN					80

// How far the user can see, in pixels
#define CK_VIEW_FRUSTUM					1000

// Max world width/	height
#define CK_MAX_WIDTH					3000000.0f
#define CK_MAX_HEIGHT					400.0f

// OpenGL scale behavior
#define CK_SCALE						100.0f

#define CK_SEA_LEVEL					600.0f
#define CK_SEA_BOTTON					0.0f

#define CK_COAST_MAX_STEP				15
#define CK_COAST_MAX_SEA_DISTANCE		(CK_COAST_MAX_STEP * 4)
#define CK_COAST_VARIATION				0.20
#define CK_COAST_BEACH_HEIGHT			25.0f
#define CK_COAST_MANGLE_HEIGHT			1

// Distance from one vertex to another
#define CK_MESH_SPACE					10

// Useful macros
#define CK_DEG2RAD(X)					((PI*(X))/180)

#endif
