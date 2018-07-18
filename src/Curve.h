// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_CURVE_H_
#define _PLANET_CURVE_H_

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "opengl.h"

class CubicSpline {
public:
    CubicSpline();
    ~CubicSpline();

    CubicSpline& addControlPoint(double x, double y);

    double operator()(double x) const;

private:
    void generateCoeffs();

    std::vector<std::pair<double, double> > m_cps;
    std::vector<double> m_coeffs;
};

class CurveDisplay {
public:
    static CurveDisplay createCurveDisplay(CubicSpline &curve, double min_x, double max_x, double min_y, double max_y, int num_points);
    ~CurveDisplay();

    void render() const;

private:
    CurveDisplay();

    void createBuffers(const std::vector<glm::vec2> &verts, const std::vector<unsigned int> &elems);
    void createProgram();
    void createArrayObject();
    
    GLuint m_array_buffer, m_elem_buffer;
    GLuint m_program;
    GLuint m_array_object;
    GLuint m_num_elems;

    GLint m_position_loc, m_color_loc;
};

#endif
