// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_OCEAN_H_
#define _PLANET_OCEAN_H_

#include <glm/mat4x4.hpp>

#include "opengl.h"

class Ocean {
public:
    static Ocean createOcean();
    ~Ocean();

    void render(const glm::mat4x4 &model, const glm::mat4x4 &view, const glm::mat4x4 &projection) const;

private:
    Ocean();

    void createBuffers();
    void createProgram();
    void createArrayObject();

    GLuint m_array_buffer, m_elem_buffer;
    GLuint m_program;
    GLuint m_array_object;
    GLuint m_num_elems;

    GLint m_position_loc, m_color_loc, m_normal_loc;
    GLint m_model_loc, m_view_loc, m_projection_loc;
};

#endif
