// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cmath>
#include <iostream>
#include <random>

#include "Noise.h"

double reduceToRange(double x, double modulus);

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

    for (int i = 256; i < 512; ++i) {
        table[i] = table[i-256];
    }
}

PermutationTable::~PermutationTable() {}

Perlin::Perlin()
    : m_permutation{}
{}

Perlin::~Perlin() {}

double Perlin::operator()(double xx, double yy) const {
    const unsigned char *const p = m_permutation.table;

    double x = reduceToRange(xx, 256.0);
    double y = reduceToRange(yy, 256.0);

    int xa = std::lround(std::floor(x));
    int xb = (xa + 1) % 256;
    int ya = std::lround(std::floor(y));
    int yb = (ya + 1) % 256;
    double xf = x - xa;
    double yf = y - ya;

    double u = Perlin::fade(xf);
    double v = Perlin::fade(yf);

    int aa = p[p[xa] + ya];
    int ab = p[p[xa] + yb];
    int ba = p[p[xb] + ya];
    int bb = p[p[xb] + yb];

    double x1 = lerp(u, grad(aa, xf, yf),   grad(ba, xf-1, yf));
    double x2 = lerp(u, grad(ab, xf, yf-1), grad(bb, xf-1, yf-1));
    return lerp(v, x1, x2);
}

double Perlin::fade(double t) {
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double Perlin::lerp(double t, double a, double b) {
    return t*(b - a) + a;
}

double Perlin::grad(int hash, double x, double y) {
    switch (hash & 0x3) {
        case 0x0: return  x +  y;
        case 0x1: return -x +  y;
        case 0x2: return  x + -y;
        case 0x3: return -x + -y;
        default : return 0;
    }
}

double reduceToRange(double x, double modulus) {
    while (x >= modulus) {
        x -= modulus;
    }

    while (x < 0) {
        x += modulus;
    }

    return x;
}
