// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_OCEAN_H_
#define _PLANET_OCEAN_H_

#include <vector>

#include "glm_defines.h"
#include <glm/mat4x4.hpp>

#include "opengl.h"

#include "Models.h"

class Ocean {
public:
    Ocean();
    Ocean(const Ocean &other) = delete;
    Ocean(Ocean &&other) = delete;
    ~Ocean();

    Ocean& operator=(const Ocean &other) = delete;
    Ocean& operator=(Ocean &&other) = delete;

    void render(const glm::mat4x4 &model) const;

private:
    void initGeometry();
    void initBuffers();
    void initProgram();
    void initVAO();

    std::vector<PCNVertex> m_vertices;
    std::vector<GLuint> m_indices;
    GLfloat m_specular_pow;

    GLuint m_array_buffer, m_elem_buffer;
    
    GLuint m_vertex_shader, m_fragment_shader, m_program;
    GLint m_position_loc, m_color_loc, m_normal_loc;
    GLint m_model_loc, m_specular_pow_loc;

    GLuint m_array_object;
};

#endif
