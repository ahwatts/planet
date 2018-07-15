// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Models.h"
#include "Noise.h"
#include "OpenGLUtils.h"
#include "Resource.h"
#include "Terrain.h"

Terrain Terrain::createTerrain() {
    OctaveNoise noise;
    Terrain rv;

    PositionsAndElements sphere = icosphere(2.0, 6);
    std::vector<PCNVertex> vertices{};
    std::vector<unsigned int> elems{};

    for (unsigned int i = 0; i < sphere.positions.size(); ++i) {
        glm::vec3 &pos = sphere.positions[i];
        pos *= noise(4, 0.3, pos.x, pos.y, pos.z) / 10.0 + 1.0;
    }

    for (unsigned int i = 0; i < sphere.elements.size(); i += 3) {
        unsigned int e1 = sphere.elements[i+0];
        unsigned int e2 = sphere.elements[i+1];
        unsigned int e3 = sphere.elements[i+2];
        const glm::vec3 &p1 = sphere.positions[e1];
        const glm::vec3 &p2 = sphere.positions[e2];
        const glm::vec3 &p3 = sphere.positions[e3];
        glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

        vertices.push_back({
            { p1.x, p1.y, p1.z },
            { 0.5, 0.5, 0.5, 1.0 },
            { normal.x, normal.y, normal.z }
        });
        elems.push_back(vertices.size() - 1);

        vertices.push_back({
            { p2.x, p2.y, p2.z },
            { 0.5, 0.5, 0.5, 1.0 },
            { normal.x, normal.y, normal.z }
        });
        elems.push_back(vertices.size() - 1);

        vertices.push_back({
            { p3.x, p3.y, p3.z },
            { 0.5, 0.5, 0.5, 1.0 },
            { normal.x, normal.y, normal.z }
        });
        elems.push_back(vertices.size() - 1);
    }

    rv.m_num_elems = elems.size();

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
    rv.m_normal_loc = glGetAttribLocation(rv.m_program, "normal");
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
      m_color_loc{-1},
      m_normal_loc{-1},
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

    glEnable(GL_DEPTH_TEST);
    glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(m_array_object);
    glDrawElements(GL_TRIANGLES, m_num_elems, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}
