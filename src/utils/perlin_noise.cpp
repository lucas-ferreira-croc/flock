#include "perlin_noise.hpp"
#include <utility>
#include <cmath>
#include <limits>
#include <random>


void PerlinNoise::SetSeed(int seed)
{
	worldSeed = seed;
	GenerateOffsets();
}

void PerlinNoise::GenerateOffsets()
{
	octaveOffsets.clear();
	octaveOffsets.reserve(octaveCount);

	std::mt19937 gen(worldSeed);
	std::uniform_real_distribution<float> d(-100000.f, 100000.f);

	for (int i = 0; i < octaveCount; i++)
	{
		octaveOffsets.push_back({ d(gen), d(gen) });
	}
}

void PerlinNoise::SetPersistance(float persistance)
{
	_persistance = persistance;
}

void PerlinNoise::SetLacunarity(float lacunarity)
{
	_lacunarity = lacunarity;
}

float PerlinNoise::Random2D(int x, int y)
{
	int n = x + y * 57 + worldSeed * 131;
	n = (n << 13) ^ n;

	int nn = (n * (n * n * 15731 + 789221) + 1376312589);

	return 1.0f - ((nn & 0x7fffffff) / 1073741824.0f);
}

float PerlinNoise::interpolatedNoise(float x, float y)
{
	int integerX = (int)floor(x);
	int integerY = (int)floor(y);

	float fractionalX = x - integerX;
	float fractionalY = y - integerY;

	float v1 = Random2D(integerX, integerY);
	float v2 = Random2D(integerX + 1, integerY);
	float v3 = Random2D(integerX, integerY + 1);
	float v4 = Random2D(integerX + 1, integerY + 1);

	float i1 = v1 + (v2 - v1) * fractionalX;
	float i2 = v3 + (v4 - v3) * fractionalX;

	return i1 + (i2 - i1) * fractionalY;
}

void PerlinNoise::SetOffset(float x, float y)
{
	offset.x = x;
	offset.y = y;
}

float PerlinNoise::Noise(float x, float y)
{
	float amplitude = 1.0f;
	float frequency = 1.0f;
	float noiseHeight= 0.0f;
	float maxValue = 0.0f;

	for(int i = 0; i < octaveCount; i++)
	{
		float sampleX = x * frequency;// + octaveOffsets[i].x + offset.x;
		float sampleY = y * frequency;// + octaveOffsets[i].y + offset.y;

		float perlinValue = interpolatedNoise(sampleX, sampleY);
		noiseHeight += perlinValue * amplitude;

		maxValue += amplitude;

		amplitude *= _persistance;
		frequency *= _lacunarity;
	}

	float result = noiseHeight / maxValue;
	return (result + 1.0f) * 0.5f;
}

void PerlinNoise::SetFrequency(int frequency)
{
	baseFrequency = frequency;
}

void PerlinNoise::SetOctave(int octave)
{
	octaveCount = octave;
	octaveOffsets.resize(octaveCount);

	GenerateOffsets();
}