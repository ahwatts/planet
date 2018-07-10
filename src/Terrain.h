// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_TERRAIN_H_
#define _PLANET_TERRAIN_H_

#include "opengl.h"

class TerrainGeometry {
public:
    TerrainGeometry();
    ~TerrainGeometry();

    void initialize();
    void dispose();

private:
    GLuint m_vertex_buffer, m_elem_buffer, m_array_object;
};

class TerrainShader {
public:
    TerrainShader();
    ~TerrainShader();

    void initialize();
    void dispose();

private:
    GLuint m_program;
};

#endif
