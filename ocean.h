//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 dhpoware. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#if !defined(OCEAN_H)
#define OCEAN_H

#include <vector>
#include "bitmap.h"
#include "mathlib.h"

class OceanHeightMap
{
public:
    OceanHeightMap();
    ~OceanHeightMap();

    float getHeightScale() const
    { return m_heightScale; }

    int getSize() const
    { return m_size; }

    int getGridSpacing() const
    { return m_gridSpacing; }

    const float *getHeights() const
    { return &m_heights[0]; }

    bool create(int size, int gridSpacing, float scale);
    void destroy();

    void generateDiamondSquareFractal(float roughness);

    float heightAt(float x, float z) const;

    float heightAtPixel(int x, int z) const
    { return m_heights[z * m_size + x]; };

    void normalAt(float x, float z, Vector3 &n) const;
    void normalAtPixel(int x, int z, Vector3 &n) const;

private:
    void blur(float amount);
    unsigned int heightIndexAt(int x, int z) const;
    void smooth();

    int m_size;
    int m_gridSpacing;
    float m_heightScale;
    std::vector<float> m_heights;
};


class Ocean
{
public:
    Ocean();
    virtual ~Ocean();

    bool create(int size, int gridSpacing, float scale);
    void destroy();
    void draw();
    bool generateUsingDiamondSquareFractal(float roughness);
    void update(const Vector3 &cameraPos);

    const OceanHeightMap &getHeightMap() const
    { return m_heightMap; }

    OceanHeightMap &getHeightMap()
    { return m_heightMap; }

protected:
    virtual bool oceanCreate(int size, int gridSpacing, float scale);
    virtual void oceanDestroy();
    virtual void oceanDraw();
    virtual void oceanUpdate(const Vector3 &cameraPos);

private:
    struct Vertex
    {
        float x, y, z;
        float nx, ny, nz;
        float s, t;
    };

    bool generateIndices();
    bool generateVertices();
    
    bool use16BitIndices() const
    { return m_totalVertices <= 65536; }

    unsigned int m_vertexBuffer;
    unsigned int m_indexBuffer;
    int m_totalVertices;
    int m_totalIndices;
    OceanHeightMap m_heightMap;
};

#endif