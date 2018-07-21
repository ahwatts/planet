// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "opengl.h"

#include "Models.h"
#include "OpenGLUtils.h"
#include "Ocean.h"
#include "Resource.h"

Ocean Ocean::createOcean() {
    Ocean rv;
    rv.createBuffers();
    rv.createProgram();
    rv.createArrayObject();
    return rv;
}

Ocean::Ocean()
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

Ocean::~Ocean() {
    std::vector<GLuint> bufs_to_delete;

    if (glIsBuffer(m_array_buffer)) {
        bufs_to_delete.push_back(m_array_buffer);
    }

    if (glIsBuffer(m_elem_buffer)) {
        bufs_to_delete.push_back(m_elem_buffer);
    }

    if (bufs_to_delete.size() > 0) {
        glDeleteBuffers(bufs_to_delete.size(), bufs_to_delete.data());
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

void Ocean::render(const glm::mat4x4 &model, const glm::mat4x4 &view, const glm::mat4x4 &projection) const {
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


void Ocean::createBuffers() {
    PositionsAndElements sphere = icosphere(1.97, 5);
    std::vector<PCNVertex> vertices(sphere.positions.size());    

    for (unsigned int i = 0; i < sphere.positions.size(); ++i) {
        glm::vec3 &pos = sphere.positions[i];
        glm::vec3 norm = glm::normalize(pos);
        vertices[i] = {
            { pos.x, pos.y, pos.z },
            { 0.2, 0.3, 0.6, 1.0 },
            { norm.x, norm.y, norm.z }
        };
    }
    
    GLuint bufs[2];
    glGenBuffers(2, bufs);
    m_array_buffer = bufs[0];
    m_elem_buffer = bufs[1];

    glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()*sizeof(PCNVertex),
        vertices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sphere.elements.size()*sizeof(unsigned int),
        sphere.elements.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_num_elems = sphere.elements.size();
}

void Ocean::createProgram() {
    Resource vert_code = LOAD_RESOURCE(ocean_vert);
    Resource frag_code = LOAD_RESOURCE(ocean_frag);

    GLuint vert_shader = createAndCompileShader(GL_VERTEX_SHADER, vert_code.toString().data());
    GLuint frag_shader = createAndCompileShader(GL_FRAGMENT_SHADER, frag_code.toString().data());
    m_program = createProgramFromShaders(vert_shader, frag_shader);
    m_position_loc = glGetAttribLocation(m_program, "position");
    m_color_loc = glGetAttribLocation(m_program, "color");
    m_normal_loc = glGetAttribLocation(m_program, "normal");
    m_model_loc = glGetUniformLocation(m_program, "model");
    m_view_loc = glGetUniformLocation(m_program, "view");
    m_projection_loc = glGetUniformLocation(m_program, "projection");

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}

void Ocean::createArrayObject() {
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
        (const void *)(0)
    );

    glEnableVertexAttribArray(m_color_loc);
    glVertexAttribPointer(
        m_color_loc,
        4, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(3*sizeof(float))
    );

    glEnableVertexAttribArray(m_normal_loc);
    glVertexAttribPointer(
        m_normal_loc,
        3, GL_FLOAT, GL_FALSE,
        sizeof(PCNVertex),
        (const void *)(7*sizeof(float))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}
