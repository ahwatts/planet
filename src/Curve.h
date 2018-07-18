// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_CURVE_H_
#define _PLANET_CURVE_H_

#include <glm/mat4x4.hpp>

#include "opengl.h"

class CurveDisplay {
public:
    static CurveDisplay createCurveDisplay(double a, double b, int num_points);
    ~CurveDisplay();

    void render(glm::mat4x4 &model, glm::mat4x4 &view, glm::mat4x4 &projection);

private:
    CurveDisplay();
    GLuint m_array_buffer, m_elem_buffer;
    GLuint m_program;
    GLuint m_array_object;
    GLuint m_num_elems;

    GLint m_position_loc, m_color_loc;
};

#endif
