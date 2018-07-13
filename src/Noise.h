// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_NOISE_H_
#define _PLANET_NOISE_H_

class PermutationTable {
public:
    PermutationTable();
    ~PermutationTable();

    unsigned char table[256];
};

class Perlin {
public:
    Perlin();
    ~Perlin();

    double operator()(double x) const;
    double operator()(double x, double y) const;
    double operator()(double x, double y, double z) const;

private:
    static double fade(double t);
    
    PermutationTable m_permutation;
};

#endif
