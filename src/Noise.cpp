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

NoiseFunction::NoiseFunction() {}

NoiseFunction::~NoiseFunction() {}

Perlin::Perlin()
    : m_permutation{}
{}

Perlin::~Perlin() {}

double Perlin::operator()(double xx, double yy) const {
    const unsigned char *const p = m_permutation.table;

    double x = reduceToRange(xx, 256.0);
    double y = reduceToRange(yy, 256.0);

    int xa = static_cast<int>(std::floor(x));
    int xb = (xa + 1) % 256;
    int ya = static_cast<int>(std::floor(y));
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

double Perlin::operator()(double xx, double yy, double zz) const {
    const unsigned char *const p = m_permutation.table;

    double x = reduceToRange(xx, 256.0);
    double y = reduceToRange(yy, 256.0);
    double z = reduceToRange(zz, 256.0);

    int xa = static_cast<int>(std::floor(x));
    int xb = (xa + 1) % 256;
    int ya = static_cast<int>(std::floor(y));
    int yb = (ya + 1) % 256;
    int za = static_cast<int>(std::floor(z));
    int zb = (za + 1) % 256;

    double xf = x - xa;
    double yf = y - ya;
    double zf = z - za;

    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    int aaa = p[p[p[xa] + ya] + za];
    int aab = p[p[p[xa] + ya] + zb];
    int aba = p[p[p[xa] + yb] + za];
    int abb = p[p[p[xa] + yb] + zb];
    int baa = p[p[p[xb] + ya] + za];
    int bab = p[p[p[xb] + ya] + zb];
    int bba = p[p[p[xb] + yb] + za];
    int bbb = p[p[p[xb] + yb] + zb];

    double x1, y1, x2, y2;
    x1 = lerp(u, grad(aaa, xf, yf, zf),   grad(baa, xf-1, yf, zf));
    x2 = lerp(u, grad(aba, xf, yf-1, zf), grad(bba, xf-1, yf-1, zf));
    y1 = lerp(v, x1, x2);

    x1 = lerp(u, grad(aab, xf, yf, zf-1),   grad(bab, xf-1, yf, zf-1));
    x2 = lerp(u, grad(abb, xf, yf-1, zf-1), grad(bbb, xf-1, yf-1, zf-1));
    y2 = lerp(v, x1, x2);

    return lerp(w, y1, y2);
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

double Perlin::grad(int hash, double x, double y, double z) {
    switch (hash & 0xF) {
        case 0x0: return  x +  y;
        case 0x1: return -x +  y;
        case 0x2: return  x + -y;
        case 0x3: return -x + -y;
        case 0x4: return  x +  z;
        case 0x5: return -x +  z;
        case 0x6: return  x + -z;
        case 0x7: return -x + -z;
        case 0x8: return  y +  z;
        case 0x9: return -y +  z;
        case 0xA: return  y + -z;
        case 0xB: return -y + -z;
        case 0xC: return  x +  y;
        case 0xD: return -x +  y;
        case 0xE: return -y +  z;
        case 0xF: return -y + -z;
        default: return 0;
    }
}

Octave::Octave(const NoiseFunction &base)
    : m_noise{base}
{}

Octave::~Octave() {}

double Octave::operator()(int octaves, double persistence, double x, double y) const {
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double max_value = 0;

    for (int i = 0; i < octaves; ++i) {
        total += m_noise(x * frequency, y * frequency) * amplitude;
        max_value += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }

    return total / max_value;
}

double Octave::operator()(int octaves, double persistence, double x, double y, double z) const {
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double max_value = 0;

    for (int i = 0; i < octaves; ++i) {
        total += m_noise(x * frequency, y * frequency, z * frequency) * amplitude;
        max_value += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }

    return total / max_value;
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
