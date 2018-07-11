// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_TERRAIN_H_
#define _PLANET_TERRAIN_H_

#include <glm/mat4x4.hpp>

#include "opengl.h"

class Terrain {
public:
    static Terrain createTerrain();
    ~Terrain();

    void render(glm::mat4x4 &model, glm::mat4x4 &view, glm::mat4x4 &projection);

private:
    Terrain();

    void createBuffers();
    void createProgram();
    void createArrayObject();

    GLuint m_array_buffer, m_elem_buffer;
    GLuint m_program;
    GLuint m_array_object;

    GLint m_position_loc, m_color_loc;
    GLint m_model_loc, m_view_loc, m_projection_loc;
};

#endif
