#ifndef MESH_EXPORTER_HPP
#define MESH_EXPORTER_HPP

#include <assimp/Exporter.hpp>
#include <assimp/Scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "log//logger.hpp"

class MeshExporter
{
public:
    static void writeMesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<int> indices)
    {
        aiMesh* mesh = new aiMesh();
        mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
        mesh->mNumVertices = vertices.size();
        mesh->mVertices = new aiVector3D[mesh->mNumVertices];

        for(size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            mesh->mVertices[i] = aiVector3D(vertices[i].x, vertices[i].y, vertices[i].z);            
        }

        mesh->mNormals = new aiVector3D[mesh->mNumVertices];
        for(size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            mesh->mNormals[i] = aiVector3D(0, 1, 0);
        }

        mesh->mTextureCoords[0] = new aiVector3D[mesh->mNumVertices];
        for(size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            mesh->mTextureCoords[0][i] = aiVector3D(uvs[i].x, uvs[i].y, 0.0f);
        }

        mesh->mNumUVComponents[0] = 2;

        mesh->mNumFaces = indices.size() / 3;
        mesh->mFaces = new aiFace[mesh->mNumFaces];
        for(unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            aiFace& face = mesh->mFaces[f];
            face.mNumIndices = 3;
            face.mIndices = new unsigned int[3];

            face.mIndices[0] = indices[f * 3 + 0];
            face.mIndices[1] = indices[f * 3 + 1];
            face.mIndices[2] = indices[f * 3 + 2];
        }

        aiScene* scene = new aiScene();
        scene->mFlags = 0;
        scene->mNumMeshes = 1;
        scene->mMeshes = new aiMesh*[1];
        scene->mMeshes[0] = mesh;

        aiMaterial* material = new aiMaterial();
        aiString path("C:\\dev\\shader\\flock\\build\\vc17\\release\\noise0.png");
        material->AddProperty(&path, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0));

        scene->mNumMaterials = 1;
        scene->mMaterials = new aiMaterial*[1];
        scene->mMaterials[0] = material;
        mesh->mMaterialIndex = 0;

        scene->mRootNode = new aiNode();
        scene->mRootNode->mNumMeshes = 1;
        scene->mRootNode->mMeshes = new unsigned int[1];
        scene->mRootNode->mMeshes[0] = 0;

        Assimp::Exporter exporter;
        auto result = exporter.Export(scene, "fbx", "scene.fbx");
        if (result != AI_SUCCESS)
        {
            Logger::err(exporter.GetErrorString());
        }
    }
};

#endif