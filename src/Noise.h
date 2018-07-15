// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_NOISE_H_
#define _PLANET_NOISE_H_

class PermutationTable {
public:
    PermutationTable();
    ~PermutationTable();

    unsigned char table[512];
};

class Perlin {
public:
    Perlin();
    ~Perlin();

    // double operator()(double x) const;
    double operator()(double x, double y) const;
    double operator()(double x, double y, double z) const;

private:
    static double fade(double t);
    static double lerp(double t, double a, double b);
    
    static double grad(int hash, double x, double y);
    static double grad(int hash, double x, double y, double z);
    
    PermutationTable m_permutation;
};

class OctaveNoise {
public:
    OctaveNoise();
    ~OctaveNoise();

    double operator()(int octaves, double persistence, double x, double y) const;
    double operator()(int octaves, double persistence, double x, double y, double z) const;

private:
    Perlin m_noise;
};

#endif
