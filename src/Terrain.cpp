// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <iostream>
#include <map>
#include <iostream>
#include <vector>

#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Curve.h"
#include "Models.h"
#include "Noise.h"
#include "OpenGLUtils.h"
#include "Resource.h"
#include "SharedBlocks.h"
#include "Terrain.h"

Terrain Terrain::createTerrain(const NoiseFunction &noise) {
    Terrain rv;
    PositionsAndElements sphere = icosphere(2.0, 5);
    std::vector<PCNVertex> vertices(sphere.positions.size());

    // Adjust the vertex positions with some noise.
    for (unsigned int i = 0; i < sphere.positions.size(); ++i) {
        glm::vec3 &pos = sphere.positions[i];
        double n = noise(pos.x, pos.y, pos.z);
        pos *= n/8.0 + 1.0;
    }

    // Build an adjacency map of vertices to triangles (as base
    // element indices).
    std::map<unsigned int, std::vector<unsigned int> > adj_map;
    for (unsigned int i = 0; i < sphere.elements.size(); i += 3) {
        adj_map[sphere.elements[i+0]].push_back(i);
        adj_map[sphere.elements[i+1]].push_back(i);
        adj_map[sphere.elements[i+2]].push_back(i);
    }

    // Compute the PCNVertex for the vertex at this position.
    for (unsigned int vid = 0; vid < sphere.positions.size(); ++vid) {
        glm::vec3 &vp = sphere.positions[vid];

        // Compute the vertex normal as a weighted average of the
        // facet normals for the triangles adjacent to this vertex.
        glm::vec3 vertex_normal{0.0f, 0.0f, 0.0f};

        for (auto tid : adj_map[vid]) {
            unsigned int vid1 = sphere.elements[tid+0];
            unsigned int vid2 = sphere.elements[tid+1];
            unsigned int vid3 = sphere.elements[tid+2];
            const glm::vec3 &v1 = sphere.positions[vid1];
            const glm::vec3 &v2 = sphere.positions[vid2];
            const glm::vec3 &v3 = sphere.positions[vid3];
            glm::vec3 cross = glm::cross(v2 - v1, v3 - v1);
            glm::vec3 face_normal = glm::normalize(cross);

            // Weight by the area (the mangitude of the cross product
            // is twice the area of the triangle). The extra factor of
            // 2 is unimportant, since we're normalizing the result.
            float area = glm::length(cross);

            // Also weight by the angle of the triangle at this
            // vertex.
            glm::vec3 s1, s2;
            if (vid == vid1) {
                s1 = v1 - v2;
                s2 = v1 - v3;
            } else if (vid == vid2) {
                s1 = v2 - v1;
                s2 = v2 - v3;
            } else if (vid == vid3) {
                s1 = v3 - v1;
                s2 = v3 - v2;
            }
            float angle = std::acos(glm::dot(s1, s2) / glm::length(s1) / glm::length(s2));

            vertex_normal += face_normal * area * angle;
        }

        vertex_normal = glm::normalize(vertex_normal);
        vertices[vid] = {
            { vp.x, vp.y, vp.z },
            { 0.2, 0.2, 0.2, 1.0 },
            { vertex_normal.x, vertex_normal.y, vertex_normal.z }
        };
    }

    rv.m_num_elems = static_cast<unsigned int>(sphere.elements.size());

    GLuint buffers[2];
    glGenBuffers(2, buffers);
    rv.m_array_buffer = buffers[0];
    rv.m_elem_buffer = buffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, rv.m_array_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()*sizeof(PCNVertex),
        vertices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rv.m_elem_buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sphere.elements.size()*sizeof(unsigned int),
        sphere.elements.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Resource vert_code = LOAD_RESOURCE(terrain_vert);
    Resource frag_code = LOAD_RESOURCE(terrain_frag);

    GLuint vert_shader = createAndCompileShader(GL_VERTEX_SHADER, vert_code.toString().data());
    GLuint frag_shader = createAndCompileShader(GL_FRAGMENT_SHADER, frag_code.toString().data());
    rv.m_program = createProgramFromShaders(vert_shader, frag_shader);
    ViewAndProjectionBlock::setOffsets(rv.m_program, "ViewAndProjectionBlock");
    LightListBlock::setOffsets(rv.m_program, "LightListBlock");
    rv.m_position_loc = 0;
    rv.m_normal_loc = 1;
    rv.m_model_loc = glGetUniformLocation(rv.m_program, "model");

    GLuint vp_block_idx = glGetUniformBlockIndex(rv.m_program, "ViewAndProjectionBlock");
    glUniformBlockBinding(rv.m_program, vp_block_idx, ViewAndProjectionBlock::BINDING_INDEX);

    GLuint light_block_idx = glGetUniformBlockIndex(rv.m_program, "LightListBlock");
    glUniformBlockBinding(rv.m_program, light_block_idx, LightListBlock::BINDING_INDEX);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    glGenVertexArrays(1, &rv.m_array_object);
    glUseProgram(rv.m_program);
    glBindVertexArray(rv.m_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, rv.m_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rv.m_elem_buffer);

    glEnableVertexAttribArray(rv.m_position_loc);
    glVertexAttribPointer(
        rv.m_position_loc,
        3, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(0)
    );

    glEnableVertexAttribArray(rv.m_normal_loc);
    glVertexAttribPointer(
        rv.m_normal_loc,
        3, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(7*sizeof(float))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    return rv;
}

Terrain::Terrain()
    : m_array_buffer{0},
      m_elem_buffer{0},
      m_program{0},
      m_array_object{0},
      m_num_elems{0},
      m_position_loc{-1},
      m_normal_loc{-1},
      m_model_loc{-1}
{}

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
        glDeleteProgram(m_program);
    }

    m_program = 0;

    if (glIsVertexArray(m_array_object)) {
        glDeleteVertexArrays(1, &m_array_object);
    }

    m_array_object = 0;
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

    glDrawElements(GL_TRIANGLES, m_num_elems, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
