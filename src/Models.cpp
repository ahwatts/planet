// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cmath>

#include "Models.h"

const double PHI = (1.0 + std::sqrt(5.0)) / 2.0;

const float ICOSAHEDRON_VERTICES[12][3] = {
    {  1.0,  PHI,  0.0 }, // 0
    { -1.0,  PHI,  0.0 }, // 1
    {  1.0, -PHI,  0.0 }, // 2
    { -1.0, -PHI,  0.0 }, // 3
    {  PHI,  0.0,  1.0 }, // 4
    {  PHI,  0.0, -1.0 }, // 5
    { -PHI,  0.0,  1.0 }, // 6
    { -PHI,  0.0, -1.0 }, // 7
    {  0.0,  1.0,  PHI }, // 8
    {  0.0, -1.0,  PHI }, // 9
    {  0.0,  1.0, -PHI }, // 10
    {  0.0, -1.0, -PHI }, // 11
};

const unsigned int ICOSAHEDRON_VERTEX_COUNT = sizeof(ICOSAHEDRON_VERTICES) / sizeof(ICOSAHEDRON_VERTICES[0]);

const unsigned int ICOSAHEDRON_ELEMS[60] = {
    1, 7, 6,
    1, 6, 8,
    1, 8, 0,
    1, 0, 10,
    1, 10, 7,
    7, 3, 6,
    6, 3, 9,
    6, 9, 8,
    8, 9, 4,
    8, 4, 0,
    0, 4, 5,
    0, 5, 10,
    10, 5, 11,
    10, 11, 7,
    7, 11, 3,
    3, 2, 9,
    9, 2, 4,
    4, 2, 5,
    5, 2, 11,
    11, 2, 3,
};

const unsigned int ICOSAHEDRON_ELEM_COUNT = sizeof(ICOSAHEDRON_ELEMS) / sizeof(ICOSAHEDRON_ELEMS[0]);
