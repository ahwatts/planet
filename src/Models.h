// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_MODELS_H_
#define _PLANET_MODELS_H_

#include <vector>

#include <glm/vec3.hpp>

struct PCNVertex {
    float position[3];
    float color[4];
    float normal[3];
};

struct PositionsAndElements {
    std::vector<glm::vec3> positions;
    std::vector<unsigned int> elements;
};

PositionsAndElements icosahedron();
PositionsAndElements icosphere(float radius, int refinements);

extern const float ICOSAHEDRON_VERTICES[12][3];
extern const unsigned int ICOSAHEDRON_VERTEX_COUNT;
extern const unsigned int ICOSAHEDRON_ELEMS[60];
extern const unsigned int ICOSAHEDRON_ELEM_COUNT;

#endif
