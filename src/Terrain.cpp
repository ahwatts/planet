// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include "Models.h"
#include "OpenGLUtils.h"
#include "Resource.h"
#include "Terrain.h"

Terrain Terrain::createTerrain() {
    Terrain rv;

    std::vector<PCNVertex> vertices{};
    std::vector<unsigned int> elems{};

    for (unsigned int i = 0; i < ICOSAHEDRON_ELEM_COUNT; i += 3) {
        unsigned int e1 = ICOSAHEDRON_ELEMS[i+0];
        unsigned int e2 = ICOSAHEDRON_ELEMS[i+1];
        unsigned int e3 = ICOSAHEDRON_ELEMS[i+2];
        const float *p1 = ICOSAHEDRON_VERTICES[e1];
        const float *p2 = ICOSAHEDRON_VERTICES[e2];
        const float *p3 = ICOSAHEDRON_VERTICES[e3];

        glm::vec3 pos1{p1[0], p1[1], p1[2]};
        glm::vec3 pos2{p2[0], p2[1], p2[2]};
        glm::vec3 pos3{p3[0], p3[1], p3[2]};
        glm::vec3 normal = glm::normalize(glm::cross(pos2 - pos1, pos3 - pos1));

        vertices.push_back({ { pos1.x, pos1.y, pos1.z }, { 0.7, 0.8, 0.7, 1.0 }, { normal.x, normal.y, normal.z } });
        elems.push_back(vertices.size() - 1);
        vertices.push_back({ { pos2.x, pos2.y, pos2.z }, { 0.7, 0.8, 0.7, 1.0 }, { normal.x, normal.y, normal.z } });
        elems.push_back(vertices.size() - 1);
        vertices.push_back({ { pos3.x, pos3.y, pos3.z }, { 0.7, 0.8, 0.7, 1.0 }, { normal.x, normal.y, normal.z } });
        elems.push_back(vertices.size() - 1);
    }

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
        elems.size()*sizeof(unsigned int),
        elems.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Resource vert_code = LOAD_RESOURCE(terrain_vert);
    Resource frag_code = LOAD_RESOURCE(terrain_frag);

    GLuint vert_shader = createAndCompileShader(GL_VERTEX_SHADER, vert_code.toString().data());
    GLuint frag_shader = createAndCompileShader(GL_FRAGMENT_SHADER, frag_code.toString().data());
    rv.m_program = createProgramFromShaders(vert_shader, frag_shader);
    rv.m_position_loc = glGetAttribLocation(rv.m_program, "position");
    rv.m_color_loc = glGetAttribLocation(rv.m_program, "color");
    rv.m_model_loc = glGetUniformLocation(rv.m_program, "model");
    rv.m_view_loc = glGetUniformLocation(rv.m_program, "view");
    rv.m_projection_loc = glGetUniformLocation(rv.m_program, "projection");

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

    glEnableVertexAttribArray(rv.m_color_loc);
    glVertexAttribPointer(
        rv.m_color_loc,
        4, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(3*sizeof(float))
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
      m_position_loc{-1},
      m_color_loc{-1},
      m_model_loc{-1},
      m_view_loc{-1},
      m_projection_loc{-1}
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
        glDeleteBuffers(bufs.size(), bufs.data());
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


void Terrain::render(glm::mat4x4 &model, glm::mat4x4 &view, glm::mat4x4 &projection) {
    glUseProgram(m_program);

    glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(m_array_object);

    glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}