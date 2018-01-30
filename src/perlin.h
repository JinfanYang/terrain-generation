
#ifndef TERRAIN_PERLIN_H
#define TERRAIN_PERLIN_H

#include "pcg32/pcg32.h"
#include <cstdint>
#include <numeric>
#include <algorithm>
#include <random>

class Perlin{
private:
    float AMPLITUDE = 100.f;
    int OCTAVES = 3;
    float ROUGHNESS = 0.5f;

    pcg32 rng;

public:
    void setOctaves(int o) {
        OCTAVES = o;
    }

    void setRoughness(float r) {
        ROUGHNESS = r;
    }

    void setAmplitude(float a) {
        AMPLITUDE = a;
    }

    float getHeight(int x, int z){
        float total = 0;

        float d = (float) pow(2, OCTAVES - 1);

        for(int i = 0;i < OCTAVES; i++){
            float freq = (float) (pow(2, i) / d);
            float amp = (float) pow(ROUGHNESS, i) * AMPLITUDE;
            total += getInterpolatedNoise(x * freq, z * freq) * amp;
        }
        return total;
    }

    float getInterpolatedNoise(float x, float z){
        int intX = (int) x;
        int intZ = (int) z;
        float fracX = x - intX;
        float fracZ = z - intZ;

        float v1 = getSmoothNoise(intX, intZ);
        float v2 = getSmoothNoise(intX + 1, intZ);
        float v3 = getSmoothNoise(intX, intZ + 1);
        float v4 = getSmoothNoise(intX + 1, intZ + 1);
        float i1 = interpolate(v1, v2, fracX);
        float i2 = interpolate(v3, v4, fracX);
        return interpolate(i1, i2, fracZ);
    }

    float interpolate(float a, float b, float blend){
        double theta = blend * M_PI;
        float f = (float)(1.0f - cos(theta)) * 0.5f;
        return a * (1.0f - f) + b * f;
    }

    float getSmoothNoise(int x, int z) {
        float corners = (getNoise(x - 1, z - 1) + getNoise(x + 1, z - 1) +
                         getNoise(x - 1, z + 1) + getNoise(x + 1, z + 1)) / 16.0f;

        float sides = (getNoise(x - 1, z) + getNoise(x + 1, z) +
                       getNoise(x, z - 1) + getNoise(x, z + 1)) / 8.0f;

        float center = getNoise(x, z) / 4.0f;
        return corners + sides + center;
    }

    float getNoise(int x, int z){
        rng.seed(x * 562, z * 673);
        return rng.nextFloat() * 2.f - 1.f;
    }

};

class Perlin2{
private:
    int *p;

    float *Gx;
    float *Gy;
    float *Gz;

    int octaves = 0;
    float lacunarity = 2.0f;
    float persistence = 0.5f;
    float frequency = 1.0f;
    float amplitude = 1.0f;

public:
    Perlin2() {
        octaves = 8;
        lacunarity = 2.0f;
        persistence = 0.5f;
        frequency = 4.0f;
        amplitude = 1.0f;
    }

    Perlin2(int o, float l, float per, float f, float a) {
        octaves = o;
        lacunarity = l;
        persistence = per;
        frequency = f;
        amplitude = a;

        srand(time(NULL));

        p = new int[256];
        Gx = new float[256];
        Gy = new float[256];
        Gz = new float[256];

        for (int i=0; i<256; ++i) {
            p[i] = i;

            Gx[i] = (float(rand()) / (RAND_MAX/2)) - 1.0f;
            Gy[i] = (float(rand()) / (RAND_MAX/2)) - 1.0f;
            Gz[i] = (float(rand()) / (RAND_MAX/2)) - 1.0f;
        }

        int j=0;
        int swp=0;
        for (int i=0; i<256; i++) {
            j = rand() & 255;

            swp = p[i];
            p[i] = p[j];
            p[j] = swp;
        }
    }

    ~Perlin2(){
        delete p;
        delete Gx;
        delete Gy;
        delete Gz;
    }

    float noise(float sample_x, float sample_y, float sample_z)
    {
        // Unit cube vertex coordinates surrounding the sample point
        int x0 = int(floorf(sample_x));
        int x1 = x0 + 1;
        int y0 = int(floorf(sample_y));
        int y1 = y0 + 1;
        int z0 = int(floorf(sample_z));
        int z1 = z0 + 1;

        // Determine sample point position within unit cube
        float px0 = sample_x - float(x0);
        float px1 = px0 - 1.0f;
        float py0 = sample_y - float(y0);
        float py1 = py0 - 1.0f;
        float pz0 = sample_z - float(z0);
        float pz1 = pz0 - 1.0f;

        // Compute dot product between gradient and sample position vector
        int gIndex = p[(x0 + p[(y0 + p[z0 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d000 = Gx[gIndex]*px0 + Gy[gIndex]*py0 + Gz[gIndex]*pz0;

        gIndex = p[(x1 + p[(y0 + p[z0 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d001 = Gx[gIndex]*px1 + Gy[gIndex]*py0 + Gz[gIndex]*pz0;

        gIndex = p[(x0 + p[(y1 + p[z0 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d010 = Gx[gIndex]*px0 + Gy[gIndex]*py1 + Gz[gIndex]*pz0;

        gIndex = p[(x1 + p[(y1 + p[z0 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d011 = Gx[gIndex]*px1 + Gy[gIndex]*py1 + Gz[gIndex]*pz0;

        gIndex = p[(x0 + p[(y0 + p[z1 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d100 = Gx[gIndex]*px0 + Gy[gIndex]*py0 + Gz[gIndex]*pz1;

        gIndex = p[(x1 + p[(y0 + p[z1 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d101 = Gx[gIndex]*px1 + Gy[gIndex]*py0 + Gz[gIndex]*pz1;

        gIndex = p[(x0 + p[(y1 + p[z1 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d110 = Gx[gIndex]*px0 + Gy[gIndex]*py1 + Gz[gIndex]*pz1;

        gIndex = p[(x1 + p[(y1 + p[z1 & 255]) & 255]) & 255];
        gIndex = abs(gIndex) % 256;
        float d111 = Gx[gIndex]*px1 + Gy[gIndex]*py1 + Gz[gIndex]*pz1;

        // Interpolate dot product values at sample point using polynomial interpolation 6x^5 - 15x^4 + 10x^3
        float wx = ((6*px0 - 15)*px0 + 10)*px0*px0*px0;
        float wy = ((6*py0 - 15)*py0 + 10)*py0*py0*py0;
        float wz = ((6*pz0 - 15)*pz0 + 10)*pz0*pz0*pz0;

        float xa = d000 + wx*(d001 - d000);
        float xb = d010 + wx*(d011 - d010);
        float xc = d100 + wx*(d101 - d100);
        float xd = d110 + wx*(d111 - d110);
        float ya = xa + wy*(xb - xa);
        float yb = xc + wy*(xd - xc);
        float value = ya + wz*(yb - ya);

        return value;
    }

    float noise3D(float sample_x, float sample_y, float sample_z) {
        float sum = 0;
        float freq = frequency;
        float amp = amplitude;

        for (int i=0; i < octaves; ++i) {
            sum += noise(sample_x*freq, sample_y*freq, sample_z*freq)*amp;

            freq *= lacunarity;
            amp *= persistence;
        }

        return sum;
    }

    float getHeight(int x, int z, int count) {
        float xf = (float) (x + 0.1) / (float) count;
        float zf = (float) (z + 0.1) / (float) count;

        return noise3D(xf, 0.0f, zf);
    }
};

#endif //TERRAIN_PERLIN_H
