// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <map>
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
#include "Terrain.h"

Terrain Terrain::createTerrain() {
    Perlin base_noise;
    Octave noise{base_noise};
    CubicSpline curve;
    Terrain rv;

    curve
        .addControlPoint(-1.0, -1.0)
        .addControlPoint(-0.5, -0.5)
        .addControlPoint(0.0, 0.0)
        .addControlPoint(0.2, 0.6)
        .addControlPoint(1.0, 1.0);

    PositionsAndElements sphere = icosphere(2.0, 6);
    std::vector<PCNVertex> vertices(sphere.positions.size());
    std::vector<unsigned int> elems{};

    // Adjust the vertex positions with some noise.
    for (unsigned int i = 0; i < sphere.positions.size(); ++i) {
        glm::vec3 &pos = sphere.positions[i];
        // double perlin_noise = base_noise(pos.x, pos.y, pos.z);
        double octave_noise = noise(4, 0.3, pos.x, pos.y, pos.z);
        double curved_noise = curve(octave_noise);
        pos *= curved_noise/10.0 + 1.0;
    }

    std::map<unsigned int, std::vector<unsigned int> > adj_map;
    for (unsigned int i = 0; i < sphere.elements.size(); i += 3) {
        adj_map[sphere.elements[i+0]].push_back(i);
        adj_map[sphere.elements[i+1]].push_back(i);
        adj_map[sphere.elements[i+2]].push_back(i);
    }

    // For each face A in the mesh...
    for (unsigned int i = 0; i < sphere.elements.size(); i += 3) {
        unsigned int ae1 = sphere.elements[i+0];
        unsigned int ae2 = sphere.elements[i+1];
        unsigned int ae3 = sphere.elements[i+2];
        const glm::vec3 &ap1 = sphere.positions[ae1];
        const glm::vec3 &ap2 = sphere.positions[ae2];
        const glm::vec3 &ap3 = sphere.positions[ae3];
        glm::vec3 a_norm = glm::normalize(glm::cross(ap2 - ap1, ap3 - ap1));

        // For each vertex in this face.
        for (auto ve : { ae1, ae2, ae3 }) {
            glm::vec3 vp = sphere.positions[ve];

            // Start with the facet normal.
            glm::vec3 normal = a_norm;
            
            std::vector<unsigned int> &adjacent = adj_map[ve];
            for (auto j : adjacent) {
                if (j != i) {
                    unsigned int be1 = sphere.elements[j+0];
                    unsigned int be2 = sphere.elements[j+1];
                    unsigned int be3 = sphere.elements[j+2];
                    const glm::vec3 &bp1 = sphere.positions[be1];
                    const glm::vec3 &bp2 = sphere.positions[be2];
                    const glm::vec3 &bp3 = sphere.positions[be3];
                    glm::vec3 b_cross = glm::cross(bp2 - bp1, bp3 - bp1);
                    float b_area = 0.5*glm::length(b_cross);
                    glm::vec3 b_norm = glm::normalize(b_cross);

                    glm::vec3 s1, s2;
                    if (ve == ae1) {
                        s1 = ap1 - ap2;
                        s2 = ap1 - ap3;
                    } else if (ve == ae2) {
                        s1 = ap2 - ap1;
                        s2 = ap2 - ap3;
                    } else if (ve == ae3) {
                        s1 = ap3 - ap1;
                        s2 = ap3 - ap2;
                    }

                    float angle = std::acos(glm::dot(s1, s2) / glm::length(s1) / glm::length(s2));
                    normal += b_norm * b_area * angle;
                }
            }

            normal = glm::normalize(normal);
            elems.push_back(vertices.size());
            vertices.push_back({
                { vp.x, vp.y, vp.z },
                { 0.5, 0.5, 0.5, 1.0 },
                { normal.x, normal.y, normal.z }
            });
        }
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
