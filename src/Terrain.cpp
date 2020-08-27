// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cstddef>
#include <iostream>
#include <vector>

#include "glm_defines.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "opengl.h"

#include "Models.h"
#include "Noise.h"
#include "OpenGLUtils.h"
#include "Resource.h"
#include "SharedBlocks.h"
#include "Terrain.h"

Terrain::Terrain()
    : m_vertices{},
      m_indices{},
      m_array_buffer{0},
      m_elem_buffer{0},
      m_vertex_shader{0},
      m_fragment_shader{0},
      m_program{0},
      m_position_loc{-1},
      m_normal_loc{-1},
      m_model_loc{-1},
      m_array_object{0}
{}

Terrain::Terrain(const NoiseFunction &noise): Terrain() {
    initGeometry(noise);
    initBuffers();
    initProgram();
    initVAO();
}

Terrain::~Terrain() {
    std::vector<GLuint> bufs{};

    if (glIsBuffer(m_array_buffer)) {
        bufs.push_back(m_array_buffer);
    }

    if (glIsBuffer(m_elem_buffer)) {
        bufs.push_back(m_elem_buffer);
    }

    if (bufs.size() > 0) {
        glDeleteBuffers(static_cast<GLsizei>(bufs.size()), bufs.data());
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

void Terrain::initGeometry(const NoiseFunction &noise) {
    // Create a sphere from an icosahredron.
    PositionsAndElements sphere = icosphere(2.0, 5);

    // Adjust the vertex positions with some noise.
    for (unsigned int i = 0; i < sphere.positions.size(); ++i) {
        glm::vec3 &pos = sphere.positions[i];
        double n = noise(pos.x, pos.y, pos.z);
        pos *= n/8.0 + 1.0;
    }

    // Compute the normals for smoothness.
    std::vector<glm::vec3> normals = computeNormals(sphere);

    m_indices = sphere.elements;
    m_vertices.resize(sphere.positions.size());
    for (std::size_t i = 0; i < sphere.positions.size(); ++i) {
        m_vertices[i].position = sphere.positions[i];
        m_vertices[i].normal = normals[i];
    }
}

void Terrain::initBuffers() {
    GLuint buffers[2];
    glGenBuffers(2, buffers);
    m_array_buffer = buffers[0];
    m_elem_buffer = buffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size()*sizeof(TerrainVertex),
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

void Terrain::initProgram() {
    Resource vert_code = LOAD_RESOURCE(terrain_vert);
    Resource frag_code = LOAD_RESOURCE(terrain_frag);

    m_vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, vert_code.toString().data());
    m_fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, frag_code.toString().data());
    m_program = createProgramFromShaders(m_vertex_shader, m_fragment_shader);
    ViewAndProjectionBlock::setOffsets(m_program, "ViewAndProjectionBlock");
    LightListBlock::setOffsets(m_program, "LightListBlock");
    m_position_loc = 0;
    m_normal_loc = 1;
    m_model_loc = glGetUniformLocation(m_program, "model");

    GLuint vp_block_idx = glGetUniformBlockIndex(m_program, "ViewAndProjectionBlock");
    glUniformBlockBinding(m_program, vp_block_idx, ViewAndProjectionBlock::BINDING_INDEX);

    GLuint light_block_idx = glGetUniformBlockIndex(m_program, "LightListBlock");
    glUniformBlockBinding(m_program, light_block_idx, LightListBlock::BINDING_INDEX);
}

void Terrain::initVAO() {
    glGenVertexArrays(1, &m_array_object);
    glUseProgram(m_program);
    glBindVertexArray(m_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);

    glEnableVertexAttribArray(m_position_loc);
    glVertexAttribPointer(
        m_position_loc,
        3, GL_FLOAT, GL_FALSE,
        sizeof(TerrainVertex),
        (const void *)(offsetof(TerrainVertex, position))
    );

    glEnableVertexAttribArray(m_normal_loc);
    glVertexAttribPointer(
        m_normal_loc,
        3, GL_FLOAT, GL_FALSE,
        sizeof(TerrainVertex),
        (const void *)(offsetof(TerrainVertex, normal))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);    
}

void Terrain::render(glm::mat4x4 &model) {
    glUseProgram(m_program);

    glEnable(GL_DEPTH_TEST);
    glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
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
