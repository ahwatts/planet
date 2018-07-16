// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_NOISE_H_
#define _PLANET_NOISE_H_

#include <vector>

class PermutationTable {
public:
    PermutationTable();
    ~PermutationTable();

    unsigned char table[512];
};

class NoiseFunction {
public:
    NoiseFunction();
    virtual ~NoiseFunction();

    // double operator()(double x) const;
    virtual double operator()(double x, double y) const = 0;
    virtual double operator()(double x, double y, double z) const = 0;
};

class Perlin : public NoiseFunction {
public:
    Perlin();
    ~Perlin();

    // double operator()(double x) const;
    virtual double operator()(double x, double y) const;
    virtual double operator()(double x, double y, double z) const;

private:
    static double fade(double t);
    static double lerp(double t, double a, double b);
    
    static double grad(int hash, double x, double y);
    static double grad(int hash, double x, double y, double z);
    
    PermutationTable m_permutation;
};

class Octave {
public:
    Octave(const NoiseFunction &base);
    ~Octave();

    double operator()(int octaves, double persistence, double x, double y) const;
    double operator()(int octaves, double persistence, double x, double y, double z) const;

private:
    const NoiseFunction &m_noise;
};

class CubicSpline {
public:
    CubicSpline();
    ~CubicSpline();

    CubicSpline& addControlPoint(double x, double y);

    double operator()(double x) const;

private:
    void generateCoeffs();

    std::vector<std::pair<double, double> > m_cps;
    std::vector<double[4]> m_coeffs;
};

#endif
