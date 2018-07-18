// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "opengl.h"

#include "Curve.h"
#include "Noise.h"

CurveDisplay CurveDisplay::createCurveDisplay(double a, double b, int num_points) {
    CubicSpline curve;
    CurveDisplay rv;

    std::vector<glm::vec2> positions;
    std::vector<unsigned int> elements;
    double h = (b - 1) / num_points;
    for (int i = 0; i < num_points + 1; ++i) {
        double x = a + i*h;
        double y = curve(x);
        elements.push_back(positions.size());
        positions.push_back({ x, y });
    }

    return rv;
}

CurveDisplay::CurveDisplay()
    : m_array_buffer{0},
      m_elem_buffer{0},
      m_program{0},
      m_array_object{0},
      m_num_elems{0},
      m_position_loc{0},
      m_color_loc{0}
{}

CurveDisplay::~CurveDisplay() {
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

