// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <vector>

#include "glm_defines.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "opengl.h"

#include "Models.h"
#include "OpenGLUtils.h"
#include "Ocean.h"
#include "Resource.h"
#include "SharedBlocks.h"

Ocean::Ocean()
    : m_vertices{},
      m_indices{},
      m_specular_pow{0.0},
      m_array_buffer{0},
      m_elem_buffer{0},
      m_vertex_shader{0},
      m_fragment_shader{0},
      m_program{0},
      m_position_loc{-1},
      m_color_loc{-1},
      m_normal_loc{-1},
      m_model_loc{-1},
      m_specular_pow_loc{-1},
      m_array_object{0}
{
    initGeometry();
    m_specular_pow = 40.0;
    initBuffers();
    initProgram();
    initVAO();
}

Ocean::~Ocean() {
    std::vector<GLuint> bufs_to_delete;

    if (glIsBuffer(m_array_buffer)) {
        bufs_to_delete.push_back(m_array_buffer);
    }

    if (glIsBuffer(m_elem_buffer)) {
        bufs_to_delete.push_back(m_elem_buffer);
    }

    if (bufs_to_delete.size() > 0) {
        glDeleteBuffers(static_cast<GLsizei>(bufs_to_delete.size()), bufs_to_delete.data());
    }

    m_array_buffer = 0;
    m_elem_buffer = 0;

    if (glIsProgram(m_program)) {
        if (glIsShader(m_vertex_shader)) {
            glDetachShader(m_program, m_vertex_shader);
            glDeleteShader(m_vertex_shader);
        }
        m_vertex_shader = 0;
        
        if (glIsShader(m_fragment_shader)) {
            glDetachShader(m_program, m_fragment_shader);
            glDeleteShader(m_fragment_shader);
        }
        m_fragment_shader = 0;
        
        glDeleteProgram(m_program);
    }

    m_program = 0;

    if (glIsVertexArray(m_array_object)) {
        glDeleteVertexArrays(1, &m_array_object);
    }

    m_array_object = 0;
}

void Ocean::render(const glm::mat4x4 &model) const {
    glUseProgram(m_program);

    glEnable(GL_DEPTH_TEST);
    glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(m_specular_pow_loc, m_specular_pow);
    glBindVertexArray(m_array_object);

    // static int i = 0;
    // if (i % 500 == 0) {
    //     dumpOpenGLState();
    // }
    // ++i;

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Ocean::initGeometry() {
    PositionsAndElements sphere = icosphere(1.97f, 5);
    m_vertices.resize(sphere.positions.size());
    m_indices = sphere.elements;

    for (unsigned int i = 0; i < sphere.positions.size(); ++i) {
        glm::vec3 &pos = sphere.positions[i];
        glm::vec3 norm = glm::normalize(pos);
        m_vertices[i] = {
            { pos.x, pos.y, pos.z },
            { 0.2f, 0.3f, 0.6f, 1.0f },
            { norm.x, norm.y, norm.z }
        };
    }
}

void Ocean::initBuffers() {
    GLuint bufs[2];
    glGenBuffers(2, bufs);
    m_array_buffer = bufs[0];
    m_elem_buffer = bufs[1];

    glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size()*sizeof(PCNVertex),
        m_vertices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_indices.size()*sizeof(GLuint),
        m_indices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Ocean::initProgram() {
    Resource vert_code = LOAD_RESOURCE(ocean_vert);
    Resource frag_code = LOAD_RESOURCE(ocean_frag);

    m_vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, vert_code.toString().data());
    m_fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, frag_code.toString().data());
    m_program = createProgramFromShaders(m_vertex_shader, m_fragment_shader);
    LightListBlock::setOffsets(m_program, "LightListBlock");
    m_position_loc = 0;
    m_color_loc = 1;
    m_normal_loc = 2;
    m_model_loc = glGetUniformLocation(m_program, "model");
    m_specular_pow_loc = glGetUniformLocation(m_program, "specular_pow");

    GLuint vp_block_idx = glGetUniformBlockIndex(m_program, "ViewAndProjectionBlock");
    glUniformBlockBinding(m_program, vp_block_idx, ViewAndProjectionBlock::BINDING_INDEX);

    GLuint light_block_idx = glGetUniformBlockIndex(m_program, "LightListBlock");
    glUniformBlockBinding(m_program, light_block_idx, LightListBlock::BINDING_INDEX);
}

void Ocean::initVAO() {
    glGenVertexArrays(1, &m_array_object);
    glUseProgram(m_program);
    glBindVertexArray(m_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);

    glEnableVertexAttribArray(m_position_loc);
    glVertexAttribPointer(
        m_position_loc,
        3, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(offsetof(PCNVertex, position))
    );

    glEnableVertexAttribArray(m_color_loc);
    glVertexAttribPointer(
        m_color_loc,
        4, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(offsetof(PCNVertex, color))
    );

    glEnableVertexAttribArray(m_normal_loc);
    glVertexAttribPointer(
        m_normal_loc,
        3, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(offsetof(PCNVertex, normal))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}
