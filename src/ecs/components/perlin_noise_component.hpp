#ifndef PERLIN_NOISE_COMPONENT_HPP
#define PERLIN_NOISE_COMPONENT_HPP

#include "utils/perlin_noise.hpp"
#include "log/logger.hpp"
#include "texture/texture.hpp"

struct PerlinNoiseComponent
{
    enum DrawMode
    {
        NoiseMap = 0,
        ColourMap
    };

    PerlinNoiseComponent(int width = 512, int height = 512,
        int seed = 84,
        int octave = 8,
        float persistance = 0.507f,
        float lacunarity = 2.5f
    )
    {
        _width = width;
        _height = height;
        _seed = seed;
        _perlinNoise.SetSeed(seed);
        _perlinNoise.SetOctave(octave);
        _perlinNoise.SetPersistance(persistance);
        _perlinNoise.SetLacunarity(lacunarity);

        TerrainType waterDeep = TerrainType{ "Water deep", 0.3f, glm::vec4(0, 136, 236, 0) };
        TerrainType waterShallow = TerrainType{ "Water Shallow", 0.4f, glm::vec4(0, 182, 236, 0) };
        TerrainType sand = TerrainType{ "Sand", 0.45f, glm::vec4(231, 230, 145, 0) };
        TerrainType grass = TerrainType{ "Grass", 0.55f, glm::vec4(56, 184, 42, 0) };
        TerrainType grass2 = TerrainType{ "Grass 2", 0.6f, glm::vec4(11, 104, 1, 0) };
        TerrainType rock = TerrainType{ "Rock", 0.7f, glm::vec4(91, 62, 49, 0) };
        TerrainType rock2 = TerrainType{ "Rock 2", 0.9f, glm::vec4(61, 37, 30, 0) };
        TerrainType snow = TerrainType{ "Snow", 1.0f, glm::vec4(255, 255, 255, 0) };

        regions.push_back(waterDeep);
        regions.push_back(waterShallow);
        regions.push_back(sand);
        regions.push_back(grass);
        regions.push_back(grass2);
        regions.push_back(rock);
        regions.push_back(rock2);
        regions.push_back(snow);
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

    void generateTexture(DrawMode drawMode)
    {
        auto noiseMap = GenerateNoiseMap(_width, _height, 33.0f);
        _texture = Texture(_width, _height);
        std::vector<unsigned char> pixels(drawMode == drawMode == DrawMode::NoiseMap ? _width * _height : _width * _height * 4);
        if(drawMode == DrawMode::NoiseMap)
        {
            for(int y = 0; y < _height; y++)
            {
                for(int x = 0; x < _width; x++)
                {
                    float v = noiseMap[y][x];
                    v = std::clamp(v, 0.0f, 1.0f);
                    pixels[y * _width + x] = static_cast<unsigned char>(v * 255.0f);
                }    
            }
            _texture.loadGreyTextureFromData(pixels.data(), std::to_string(_seed));
        }
        else if (drawMode == DrawMode::ColourMap)
        {
            for (int y = 0; y < _height; y++)
            {
                for(int x = 0; x < _width; x++)
                {
                    float currentHeight = noiseMap[y][x];
                    for(int i = 0; i < regions.size(); i++)
                    {
                        if(currentHeight <= regions[i].height)
                        {
                            glm::vec4 colour = regions[i].colour;
                            int index = (y * _width + x) * 4;
                            pixels[index + 0] = (unsigned char)colour.r;
                            pixels[index + 1] = (unsigned char)colour.g;
                            pixels[index + 2] = (unsigned char)colour.b;
                            pixels[index + 3] = 255;
                            break;
                        }
                    }
                }
            }
            _texture.loadTextureFromData(pixels.data(), std::to_string(_seed));
        }
    }

    void setSeed(int seed)
    {
        _perlinNoise.SetSeed(seed);
        _seed = seed;
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

    int _seed;
    int _width;
    int _height;
    Texture _texture;
    PerlinNoise _perlinNoise; 
    struct TerrainType
    {
        std::string name;
        float height;
        glm::vec4 colour;
    };
    std::vector<TerrainType> regions;
};

#endif