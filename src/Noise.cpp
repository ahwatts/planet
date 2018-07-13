// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cmath>
#include <iostream>
#include <random>

#include "Noise.h"

PermutationTable::PermutationTable() {
    std::random_device seed;
    std::default_random_engine engine{seed()};

    for (int i = 0; i < 256; ++i) {
        table[i] = i;
    }

    for (int i = 255; i > 0; --i) {
        std::uniform_int_distribution<int> random_int{0, i};
        int sucker = random_int(engine);
        std::swap(table[sucker], table[i]);
    }
}

PermutationTable::~PermutationTable() {}

Perlin::Perlin()
    : m_permutation{}
{}

Perlin::~Perlin() {}

double Perlin::fade(double t) {
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}
