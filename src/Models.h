// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_MODELS_H_
#define _PLANET_MODELS_H_

struct PCNVertex {
    float position[3];
    float color[4];
    float normal[3];
};

extern const float ICOSAHEDRON_VERTICES[12][3];
extern const unsigned int ICOSAHEDRON_VERTEX_COUNT;
extern const unsigned int ICOSAHEDRON_ELEMS[60];
extern const unsigned int ICOSAHEDRON_ELEM_COUNT;

#endif
