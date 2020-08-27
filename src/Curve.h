// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_CURVE_H_
#define _PLANET_CURVE_H_

#include <vector>

#include "glm_defines.h"
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
    CurveDisplay(const CubicSpline &curve, double min_x, double max_x, double min_y, double max_y, int num_points);
    CurveDisplay(const CurveDisplay &other) = delete;
    CurveDisplay(CurveDisplay &&other) = delete;
    ~CurveDisplay();

    CurveDisplay& operator=(const CurveDisplay &other) = delete;
    CurveDisplay& operator=(CurveDisplay &&other) = delete;

    void render() const;

private:
    CurveDisplay();

    void initGeometry(const CubicSpline &curve, double min_x, double max_x, double min_y, double max_y, int num_points);
    void initBuffer();
    void initProgram();
    void initVAO();

    std::vector<glm::vec2> m_vertices;
    
    GLuint m_array_buffer;
    
    GLuint m_vertex_shader, m_fragment_shader, m_program;
    GLint m_position_loc;
    
    GLuint m_array_object;
};

#endif
