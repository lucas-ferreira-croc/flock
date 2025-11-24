#ifndef ClothComponent_COMPONENT_HPP
#define ClothComponent_COMPONENT_HPP

#include "ecs/components/mesh_component.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

struct ClothComponent
{
    struct Edge {
        int id0;
        int id1;
        int edgeNr;
    };

    static inline void vecAdd(std::vector<float>& a, int aNr, std::vector<float>&  b, int bNr, float scale)
    {
        aNr *= 3; bNr *= 3;
        a[aNr++] += b[bNr++] * scale;
        a[aNr++] += b[bNr++] * scale;
        a[aNr]   += b[bNr  ] * scale;
    }
   
    static inline void vecAdd(std::vector<float>& a, int aNr, glm::vec3 b, float scale)
    {   
        aNr *= 3;
        float bX = b.x;
        float bY = b.y;
        float bZ = b.z; 
        a[aNr++] += bX * scale;
        a[aNr++] += bY * scale;
        a[aNr]   += bZ * scale;
    }

    static void inline vecCopy(std::vector<float>& a, int aNr, std::vector<float>& b, int bNr)
    {
        aNr *= 3; bNr *= 3;
        a[aNr++] = b[bNr++];
        a[aNr++] = b[bNr++];
        a[aNr]   = b[bNr];
    }

    static inline void vecSub(std::vector<float>& dst, int dNr, std::vector<float>& a, int aNr, std::vector<float>& b, int bNr, float scale = 1.0f)
    {
        dNr *= 3; aNr *= 3; bNr *= 3;

        dst[dNr++] = (a[aNr++] - b[bNr++]) * scale;
        dst[dNr++] = (a[aNr++] - b[bNr++]) * scale;
        dst[dNr  ] = (a[aNr++] - b[bNr]  ) * scale;
    }

    static inline void vecSub(float* dst, int dNr, std::vector<float>&  a, int aNr, std::vector<float>&  b, int bNr, float scale = 1.0f)
    {
        dNr *= 3; aNr *= 3; bNr *= 3;

        dst[dNr++] = (a[aNr++] - b[bNr++]) * scale;
        dst[dNr++] = (a[aNr++] - b[bNr++]) * scale;
        dst[dNr  ] = (a[aNr++] - b[bNr]  ) * scale;
    }

    static inline float vecDot(const float* a, const float* b)
    {
        return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    }

    static inline void vecCross(float* dst, const float* a, const float* b)
    {
        dst[0] = a[1]*b[2] - a[2]*b[1];
        dst[1] = a[2]*b[0] - a[0]*b[2];
        dst[2] = a[0]*b[1] - a[1]*b[0];
    }

    static inline float _vecDistSquared(std::vector<float>& a, int aNr, std::vector<float>& b, int bNr)
    {
        aNr *= 3; bNr *= 3;
        auto a0 = a[aNr] - b[bNr];
        auto a1 = a[aNr + 1] - b[bNr + 1];
        auto a2 = a[aNr + 2] - b[bNr + 2];
        return a0 * a0 + a1 * a1 + a2 * a2;
    }

    static inline float vecLengthSquared(std::vector<float>& a, int aNr)
    {
        aNr *= 3;
        float a0 = a[aNr];
        float a1 = a[aNr + 1];
        float a2 = a[aNr + 2];
        return a0 * a0 + a1 * a1 + a2 * a2;

    }

    static void vecScale(std::vector<float>& a, int aNr, float scale)
    {
        aNr *= 3;
        a[aNr++] *= scale;
        a[aNr++] *= scale;
        a[aNr] *= scale;
    }

    std::vector<int> findTriNeighboors(const std::vector<int>& triangleIds)
    {
        std::vector<Edge> edges;
        int numTris = triangleIds.size() / 3;

        for (int i = 0; i < numTris; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                int id0 = triangleIds[3 * i + j];
                int id1 = triangleIds[3 * i + (j + 1) % 3];

                Edge e;
                if (id0 < id1) { e.id0 = id0; e.id1 = id1; }
                else           { e.id0 = id1; e.id1 = id0; }
                e.edgeNr = 3 * i + j;

                edges.push_back(e);
            }
        }
        Logger::warning("EDGES SIZE = " + std::to_string(edges.size()));

        std::sort(edges.begin(), edges.end(), [&](const Edge& a, const Edge& b) {
            return (a.id0 < b.id0) || (a.id0 == b.id0 && a.id1 < b.id1);
        });


        
        std::vector<int> neighbors(3 * numTris, -1);
        int nr = 0;
        while (nr < edges.size())
        {
            auto e0 = edges[nr];
            nr++;
            if (nr < edges.size())
            {
                auto e1 = edges[nr];
                if (e0.id0 == e1.id0 && e0.id1 == e1.id1) {
                    neighbors[e0.edgeNr] = e1.edgeNr;
                    neighbors[e1.edgeNr] = e0.edgeNr;
                }
                nr++;
            }
        }
        Logger::warning("NEIGHBORS " + std::to_string(neighbors.size()));
        return neighbors;
    }

    void initPhysics(const std::vector<int>& faceIds)
    {
        int numTris = faceIds.size() / 3;
        float e0[3], e1[3], c[3];

        for (int i = 0; i < numTris; i++)
        {
            int id0 = faceIds[3 * i];
            int id1 = faceIds[3 * i + 1];
            int id2 = faceIds[3 * i + 2];

            vecSub(e0, 0, this->_positions, id1, this->_positions, id0);
            vecSub(e1, 0, this->_positions, id2, this->_positions, id0);
            vecCross(c, e0, e1);

            float A = 0.5f * std::sqrt(vecDot(c, c));
            float pInvMass = (A > 0.0f) ? (1.0f / (A * 3.0f)) : 0.0f;

            _inverseMasses[id0] += pInvMass;
            _inverseMasses[id1] += pInvMass;
            _inverseMasses[id2] += pInvMass;
        }

        for (int i = 0; i < _stretchingLengths.size(); i++)
        {
            int id0 = _stretchingIds[2 * i];
            int id1 = _stretchingIds[2 * i + 1];

            _stretchingLengths[i] = std::sqrt(_vecDistSquared(this->_positions, id0, this->_positions, id1));
        }

        for (int i = 0; i < _bendingLengths.size(); i++)
        {
            int id0 = _bendingIds[4 * i + 2];
            int id1 = _bendingIds[4 * i + 3];

            _bendingLengths[i] = std::sqrt(_vecDistSquared(this->_positions, id0, this->_positions, id1));
        }

        float minX = std::numeric_limits<float>::infinity();
        float maxX = -std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();

        for (int i = 0; i < _numParticles; i++)
        {
            float x = _positions[3 * i + 0];
            float y = _positions[3 * i + 1];

            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
        }

        float epsilon = 0.0001f;

        for (int i = 0; i < _numParticles; i++)
        {
            float x = _positions[3 * i + 0];
            float y = _positions[3 * i + 1];

            // if ((y > maxY - epsilon) && (x < minX + epsilon || x > maxX - epsilon))
            //     _inverseMasses[i] = 0.0f;
        }
    }

    ClothComponent(std::shared_ptr<Model> model = std::make_shared<Model>("C:\\dev\\shader\\flock\\assets\\models\\plane1.fbx"),
                   float bendingCompliance = 1.0f)
    {
        _numParticles = 0;
        std::vector<int> triangleIds;

        for (auto& mesh : model->getMeshes())
        {
            _numParticles += mesh.vertices.size();

            for (auto& vertex : mesh.vertices)
            {
                _positions.push_back(vertex.position.x);
                _positions.push_back(vertex.position.y);
                _positions.push_back(vertex.position.z);
            }

            for (auto index : mesh.indices)
                triangleIds.push_back(index);
        }

        _prevPositions = _positions;
        _restPositions = _positions;
        _velocities = std::vector<float>(_numParticles * 3, 0.0f);
        _inverseMasses = std::vector<float>(_numParticles, 0.0f);

        _neightboors = findTriNeighboors(triangleIds);
        int numTriangles = triangleIds.size() / 3;

        std::vector<int> edgeIds;
        std::vector<int> trianglePairIds;

        for (int i = 0; i < numTriangles; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                int id0 = triangleIds[3 * i + j];
                int id1 = triangleIds[3 * i + (j + 1) % 3];

                int n = _neightboors[3 * i + j];
                if (n < 0 || id0 < id1)
                {
                    edgeIds.push_back(id0);
                    edgeIds.push_back(id1);
                }

                if (n >= 0)
                {
                    int ni = std::floor(n / 3);
                    int nj = n % 3;
                    int id2 = triangleIds[3 * i + (j + 2) % 3];
                    int id3 = triangleIds[3 * ni + (nj + 2) % 3];
                    trianglePairIds.push_back(id0);
                    trianglePairIds.push_back(id1);
                    trianglePairIds.push_back(id2);
                    trianglePairIds.push_back(id3);
                    std::cout << "aqui em\n";
                }
            }
        }

        _stretchingIds = edgeIds;
        _stretchingLengths = std::vector<float>(_stretchingIds.size() / 2);
        std::cout << "trianglePairIds = " << trianglePairIds.size() << "\n";
        _bendingIds = trianglePairIds;
        std::cout << _bendingIds.size() << "\n";
        _bendingLengths = std::vector<float>(_bendingIds.size() / 4);

        _stretchingCompliance = 0.0f;
        _bendingCompliance = bendingCompliance;

        _temp.resize(4 * 3);
        _grads.resize(4 * 3);
        
        Logger::warning("stretching IDS = " + std::to_string(_stretchingIds.size()));
        Logger::warning("_stretchingLengths.size() = " + std::to_string(_stretchingLengths.size()));
        Logger::warning("_bendingIds.size() = " + std::to_string(_bendingIds.size()));
        Logger::warning("_bendingLengths.size() = " + std::to_string(_bendingLengths.size()));;
        Logger::warning("num particles = " + std::to_string(this->_numParticles));
        Logger::warning("num positions = " + std::to_string(this->_positions.size()));
        initPhysics(triangleIds);
    }


    void solveGroundCollisions()
    {
        for(int i = 0; i < this->_numParticles; i++)
        {
            if(this->_inverseMasses[i] == 0.0f)
                continue;
            float y = this->_positions[3 * i + 1];
            if(y < 0.5f * this->thickness)
            {
                float damping = 1.0f;
                vecSub(this->_grads, 0, this->_positions, i, this->_prevPositions, i);
                vecAdd(this->_positions, i, this->_grads, 0, -damping);
                this->_positions[3 * i + 1] = 0.5f * this->thickness;
            }
        }
    }

    void preSolve(float dt, glm::vec3 gravity)
    {
        for(int i = 0; i < this->_numParticles; i++)
        {
            if(this->_inverseMasses[i] == 0)
                continue;
            vecAdd(this->_velocities, i, gravity, dt);
            vecCopy(this->_prevPositions, i, this->_positions, i);
            vecAdd(this->_positions, i, this->_velocities, i, dt);
            float y = this->_positions[3 * i + 1];
        }
    }

    void solve(float dt)
    {
        this->solveStretching(this->_stretchingCompliance, dt);
        this->solveBending(this->_stretchingCompliance, dt);
    }

    void postSolve(float dt)
    {
        for(int i = 0; i < this->_numParticles; i++)
        {
            if(this->_inverseMasses[i] == 0.0f)
                continue;
            vecSub(this->_velocities, i, this->_positions, i, this->_prevPositions, i, 1.0f / dt);
        }
    }

    void solveStretching(float compliance, float dt)
    {
        float alpha = compliance /( dt * dt);

        for(int i = 0; i < this->_stretchingLengths.size(); i++)
        {
            int id0 = this->_stretchingIds[2 * i];
            int id1 = this->_stretchingIds[2 * i + 1];
            float w0 = this->_inverseMasses[id0];
            float w1 = this->_inverseMasses[id1];
            float w = w0 + w1;
            if(w == 0.0)
                continue;
            
            vecSub(this->_grads, 0, this->_positions, id0, this->_positions, id1);
            float len = std::sqrt(vecLengthSquared(this->_grads, 0));
            if(len == 0.0)
                continue;
            vecScale(this->_grads, 0, 1.0 / len);
            float restLength = this->_stretchingLengths[i];
            float C = len - restLength;
            float s = -C / (w + alpha);
            vecAdd(this->_positions, id0, this->_grads, 0, s * w0);
            vecAdd(this->_positions, id1, this->_grads, 0, -s  * w1);
        }
    }

    void solveBending(float compliance, float dt)
    {
        float alpha = compliance /( dt * dt);
        for(int i = 0; i < this->_bendingLengths.size(); i++)
        {
            int id2 = this->_bendingIds[4 * i + 2];
            int id3 = this->_bendingIds[4 * i + 3];
            float w0 = this->_inverseMasses[id2];
            float w1 = this->_inverseMasses[id3];
            float w = w0 + w1;
            if(w == 0.0)
                continue;
            
                vecSub(this->_grads, 0, this->_positions, id2, this->_positions, id3);
            float len = std::sqrt(vecLengthSquared(this->_grads, 0));
            if(len == 0.0)
                continue;
            vecScale(this->_grads, 0, 1.0 / len);
            float restLength = this->_bendingLengths[i];
            float C = len - restLength;
            float s = -C / (w + alpha);
            vecAdd(this->_positions, id2, this->_grads, 0, s * w0);
            vecAdd(this->_positions, id3, this->_grads, 0, -s  * w1);
            
        }
    }


    int _numParticles;
    std::vector<float> _positions;
    std::vector<float> _prevPositions;
    std::vector<float> _restPositions;
    std::vector<float> _velocities;
    std::vector<float> _inverseMasses;

    std::vector<int> _neightboors;
    std::vector<int> _stretchingIds;
    std::vector<float> _stretchingLengths;
    float _stretchingCompliance;

    std::vector<int> _bendingIds;
    std::vector<float> _bendingLengths;
    float _bendingCompliance;

    std::vector<float> _temp;
    std::vector<float> _grads;
    float thickness = 0.01f;
};

#endif
