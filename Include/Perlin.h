#pragma once

#include <vector>
#include <cmath>
#include <random>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

class PerlinNoise {
private:
    std::vector<int> permutation;
    
    double fade(double t) const {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    
    double lerp(double t, double a, double b) const {
        return a + t * (b - a);
    }
    
    double grad(int hash, double x, double y, double z) const {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
    
public:
    PerlinNoise(unsigned int seed = 0) {
        permutation.resize(256);
        
        for (int i = 0; i < 256; ++i) {
            permutation[i] = i;
        }
        
        boost::random::mt19937 rng(seed);
        boost::random::uniform_int_distribution<> dist(0, 255);
        
        for (int i = 255; i > 0; --i) {
            int j = dist(rng) % (i + 1);
            std::swap(permutation[i], permutation[j]);
        }
        
        permutation.insert(permutation.end(), permutation.begin(), permutation.end());
    }
    
    double noise(double x, double y, double z) const {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        int Z = static_cast<int>(std::floor(z)) & 255;
        
        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);
        
        double u = fade(x);
        double v = fade(y);
        double w = fade(z);
        
        int A = permutation[X] + Y;
        int AA = permutation[A] + Z;
        int AB = permutation[A + 1] + Z;
        int B = permutation[X + 1] + Y;
        int BA = permutation[B] + Z;
        int BB = permutation[B + 1] + Z;
        
        return lerp(w, lerp(v, lerp(u, grad(permutation[AA], x, y, z),
                                       grad(permutation[BA], x - 1, y, z)),
                              lerp(u, grad(permutation[AB], x, y - 1, z),
                                       grad(permutation[BB], x - 1, y - 1, z))),
                      lerp(v, lerp(u, grad(permutation[AA + 1], x, y, z - 1),
                                       grad(permutation[BA + 1], x - 1, y, z - 1)),
                              lerp(u, grad(permutation[AB + 1], x, y - 1, z - 1),
                                       grad(permutation[BB + 1], x - 1, y - 1, z - 1))));
    }
    
    double octaveNoise(double x, double y, double z, int octaves, double persistence) const {
        if (octaves <= 0) return 0.0;
        
        double total = 0;
        double frequency = 1;
        double amplitude = 1;
        double maxValue = 0;
        
        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2;
        }
        
        return total / maxValue;
    }
};