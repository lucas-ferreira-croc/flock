#ifndef PERLIN_NOISE_HPP
#define PERLIN_NOISE_HPP

#include <random>

struct Vec2
{
	float x;
	float y;
};

class PerlinNoise
{
public:
    void SetSeed(int seed);
    void SetOffset(float x, float y);
    void SetPersistance(float persistance);
    void SetLacunarity(float lacunarity);
    void SetFrequency(int frequency);
    void SetOctave(int octave);
    float Noise(float x, float y);

private:
    float interpolatedNoise(float x, float y);
    float Random2D(int x, int y);
    void GenerateOffsets();

    Vec2 offset;
    std::vector<Vec2> octaveOffsets;
    float* noiseSeed2D = nullptr;
    int octaveCount = 8;
    float scalingBias = 2.0f;
    int worldSeed = 0;
    int baseFrequency = 2;
    float _persistance = 0;
    float _lacunarity = 0;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
};

#endif