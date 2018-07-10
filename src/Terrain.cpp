// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <vector>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "Models.h"
#include "Terrain.h"

TerrainGeometry::TerrainGeometry()
    : m_vertex_buffer{0},
      m_elem_buffer{0},
      m_array_object{0}
{}

void TerrainGeometry::initialize() {
    dispose();

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
    m_vertex_buffer = buffers[0];
    m_elem_buffer = buffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()*sizeof(PCNVertex),
        vertices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        elems.size()*sizeof(unsigned int),
        elems.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TerrainGeometry::dispose() {
    std::vector<GLuint> bufs{};
    
    if (glIsBuffer(m_vertex_buffer)) {
        bufs.push_back(m_vertex_buffer);
    }

    if (glIsBuffer(m_elem_buffer)) {
        bufs.push_back(m_elem_buffer);
    }

    if (bufs.size() > 0) {
        glDeleteBuffers(bufs.size(), bufs.data());
    }

    m_vertex_buffer = 0;
    m_elem_buffer = 0;

    if (glIsVertexArray(m_array_object)) {
        glDeleteVertexArrays(1, &m_array_object);
    }

    m_array_object = 0;
}
