// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_TERRAIN_H_
#define _PLANET_TERRAIN_H_

#include <glm/mat4x4.hpp>

#include "opengl.h"

#include "Noise.h"

class Terrain {
public:
    static Terrain createTerrain(const NoiseFunction &noise);
    ~Terrain();

    void render(glm::mat4x4 &model, glm::mat4x4 &view, glm::mat4x4 &projection);

private:
    Terrain();

    GLuint m_array_buffer, m_elem_buffer;
    GLuint m_program;
    GLuint m_array_object;
    GLuint m_num_elems;

    GLint m_position_loc, m_normal_loc;
    GLint m_model_loc, m_view_loc, m_projection_loc;
};

#endif
