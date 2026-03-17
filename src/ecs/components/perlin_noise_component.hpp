#ifndef PERLIN_NOISE_COMPONENT_HPP
#define PERLIN_NOISE_COMPONENT_HPP

#include "utils/perlin_noise.hpp"
#include "log/logger.hpp"
#include "texture/texture.hpp"

struct PerlinNoiseComponent
{
    
    PerlinNoiseComponent(int width = 512, int height = 512,
        int seed = 0,
        int octave = 8,
        float persistance = 0.557,
        float lacunarity = 2.7
    )
    {
        _width = width;
        _height = height;
        _perlinNoise.SetSeed(seed);
        _perlinNoise.SetOctave(octave);
        _perlinNoise.SetPersistance(persistance);
        _perlinNoise.SetLacunarity(lacunarity);
    }

    std::vector<std::vector<float>> GenerateNoiseMap(int mapWidth, int mapHeight, float scale)
    {
        std::vector<std::vector<float>> noiseMap(mapWidth, std::vector<float>(mapHeight));
        if(scale <= 0)
        {
            scale = 0.0001f;
        }

        float halfWidth = mapWidth / 2.0f;
        float halfHeight = mapHeight / 2.0f;

        for(int y = 0; y < mapHeight; y++)
        {
            for(int x = 0; x < mapWidth; x++)
            {
                float sampleX = (x - halfWidth) / scale;
                float sampleY = (y - halfHeight) / scale;

                float perlinValue = _perlinNoise.Noise(sampleX, sampleY);
                noiseMap[y][x] = perlinValue; 
            }
        }

        return noiseMap;
    }

    void generateTexture()
    {
        auto noiseMap = GenerateNoiseMap(_width, _height, 33.0f);
        _texture = Texture(_width, _height);
        std::vector<unsigned char> pixels(_width * _height);

        for(int y = 0; y < _height; y++)
        {
            for(int x = 0; x < _width; x++)
            {
                float v = noiseMap[y][x];
                v = std::clamp(v, 0.0f, 1.0f);
                pixels[y * _width + x] = static_cast<unsigned char>(v * 255.0f);
            }    
        }

        _texture.loadTextureFromData(pixels.data());
    }

    void setSeed(int seed)
    {
        _perlinNoise.SetSeed(seed);
    }

    void setOctave(int octave)
    {
        _perlinNoise.SetOctave(octave);
    }

    void setFrequency(int frequency)
    {
        _perlinNoise.SetFrequency(frequency);
    }

    void setPersistance(float persistance)
    {
        _perlinNoise.SetPersistance(persistance);
    }

    void setLacunarity(float lacunarity)
    {
        _perlinNoise.SetLacunarity(lacunarity);

    }

    void setOffset(float x, float y)
    {
        _perlinNoise.SetOffset(x, y);
    }

    int _width;
    int _height;
    Texture _texture;
    PerlinNoise _perlinNoise; 
};

#endif