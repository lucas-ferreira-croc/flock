#ifndef PERLIN_NOISE_COMPONENT_HPP
#define PERLIN_NOISE_COMPONENT_HPP

#include "utils/perlin_noise.hpp"
#include "utils/mesh_exporter.hpp"
#include "log/logger.hpp"
#include "texture/texture.hpp"


class MeshData
{
public:
    MeshData(int meshWidth, int meshHeight)
    {
        vertices.resize(meshWidth * meshHeight);
        uvs.resize(meshWidth * meshHeight);
        triangles.resize((meshWidth - 1) * (meshHeight - 1) * 6);
        triangleIndex = 0;
    }

    void addTriangles(int v1, int v2, int v3)
    {
        triangles[triangleIndex    ] = v1;
        triangles[triangleIndex + 1] = v2;
        triangles[triangleIndex + 2] = v3;
        triangleIndex += 3;
    }
public:
    std::vector<glm::vec3> vertices;
    std::vector<int> triangles;
    std::vector<glm::vec2> uvs;
    int triangleIndex;
};

struct TerrainComponent
{
    enum DrawMode
    {
        NoiseMap = 0,
        ColourMap
    };

    TerrainComponent(int width = 512, int height = 512,
        int seed = 0,
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
        std::vector<std::vector<float>> noiseMap(mapHeight, std::vector<float>(mapWidth));
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

    void generateTerrainMesh()
    {
        auto noiseMap = GenerateNoiseMap(_width, _height, 33.0f);
        auto meshData = generateTerrainMesh(noiseMap);
        MeshExporter::writeMesh(meshData.vertices, meshData.uvs, meshData.triangles);
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

private:
    MeshData generateTerrainMesh(std::vector<std::vector<float>> heightMap)
    {
        float topLeftX = (_width - 1) / -2.0f;
        float topLeftZ = (_height - 1) /  2.0f;

        MeshData meshData(_width, _height);
        int vertexIndex = 0;

        for(int y = 0; y < _height; y++)
        {
            for(int x = 0; x < _width; x++)
            {
                meshData.vertices[vertexIndex] = glm::vec3(topLeftX + x, heightMap[y][x] * 80.0f , topLeftZ - y);
                meshData.uvs[vertexIndex] = glm::vec2(x / (float)_width, 1.0f - (y / (float)_height));
                
                   
                if(x < _width - 1 && y < _height - 1)
                {
                    meshData.addTriangles(vertexIndex, vertexIndex + _width + 1 , vertexIndex + _width);
                    meshData.addTriangles(vertexIndex + _width + 1, vertexIndex, vertexIndex + 1);
                }

                vertexIndex++;
            }
        }

        return meshData;
    }

public:
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