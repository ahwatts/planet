// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_OCEAN_H_
#define _PLANET_OCEAN_H_

#include "glm_defines.h"
#include <glm/mat4x4.hpp>

#include "opengl.h"

class Ocean {
public:
    static Ocean createOcean();
    ~Ocean();

    void render(const glm::mat4x4 &model) const;

private:
    Ocean();

    void createBuffers();
    void createProgram();
    void createArrayObject();

    GLuint m_array_buffer, m_elem_buffer;
    GLfloat m_specular_pow;
    GLuint m_program;
    GLuint m_array_object;
    GLuint m_num_elems;

    GLint m_position_loc, m_color_loc, m_normal_loc;
    GLint m_model_loc;
    GLint m_specular_pow_loc;
};

#endif
