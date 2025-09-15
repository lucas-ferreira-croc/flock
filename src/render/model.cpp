#include "model.hpp"

#include "texture/texture.hpp"

void Model::render(Shader& shader)
{
    for(unsigned int i = 0; i < _meshes.size(); i++)
    {
        _meshes[i].render(shader);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        Logger::err("ERROR::ASSIMP::" +  std::string(importer.GetErrorString()));
        return;
    }

    _directory = path.substr(0, path.find_last_of('\\'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(processMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
      std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<MeshTexture> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; 
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
                // tangent
                // vector.x = mesh->mTangents[i].x;
                // vector.y = mesh->mTangents[i].y;
                // vector.z = mesh->mTangents[i].z;
                // vertex.Tangent = vector;
                // // bitangent
                // vector.x = mesh->mBitangents[i].x;
                // vector.y = mesh->mBitangents[i].y;
                // vector.z = mesh->mBitangents[i].z;
                // vertex.Bitangent = vector;
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }
        // process materials
        
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN
    
            std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // // 2. specular maps
            std::vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // // 3. normal maps
            std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // // 4. height maps
            std::vector<MeshTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
}

std::vector<MeshTexture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
    std::vector<MeshTexture> textures;
    for(unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < _texturesLoaded.size(); j++)
        {
            if(std::strcmp(_texturesLoaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(_texturesLoaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        {
            MeshTexture meshTexture;
            meshTexture.texture = textureFromFile(str.C_Str(), _directory);
            meshTexture.id = meshTexture.texture->textureObject;
            meshTexture.type = typeName;
            meshTexture.path = str.C_Str();
            textures.push_back(meshTexture);
            _texturesLoaded.push_back(meshTexture);
        }
    }

    return textures;
}

std::shared_ptr<Texture> Model::textureFromFile(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    filename = directory + '\\' + filename;

    std::shared_ptr<Texture> texture = std::make_shared<Texture>(GL_TEXTURE_2D, filename);
    texture->loadTextureA();
    return texture;
}

void Model::getBoundingBox(CollisionVertices& vertices)
{
    glm::vec3 min, max;
    min = max = _meshes.at(0).vertices.at(0).position;

    for(auto mesh : _meshes)
    {
        for(auto vertex : mesh.vertices)
        {
            min.x = std::min(min.x, vertex.position.x);
            min.y = std::min(min.y, vertex.position.y);
            min.z = std::min(min.z, vertex.position.z);

            max.x = std::max(max.x, vertex.position.x);
            max.y = std::max(max.y, vertex.position.y);
            max.z = std::max(max.z, vertex.position.z);
        }
    }

    vertices.m_PositionsBuffer.reserve(9);

    glm::vec3 A;
    A.x = min.x;
    A.y = max.y;
    A.z = max.z;

    A = glm::vec4(A, 1.0f);
    vertices.m_PositionsBuffer.push_back(A);

    glm::vec3 B;
    B.x = max.x;
    B.y = min.y;
    B.z = max.z;
    B = glm::vec4(B, 1.0f);
    vertices.m_PositionsBuffer.push_back(B);

    glm::vec3 C;
    C.x = min.x;
    C.y = min.y;
    C.z = max.z;
    C = glm::vec4(C, 1.0f);
    vertices.m_PositionsBuffer.push_back(C);

    glm::vec3 D;
    D.x = max.x;
    D.y = max.y;
    D.z = max.z;
    D = glm::vec4(D, 1.0f);
    vertices.m_PositionsBuffer.push_back(D);

    glm::vec3 E;
    E.x = max.x;
    E.y = min.y;
    E.z = min.z;
    E = glm::vec4(E, 1.0f);
    vertices.m_PositionsBuffer.push_back(E);

    glm::vec3 F;
    F.x = max.x;
    F.y = max.y;
    F.z = min.z;
    F = glm::vec4(F, 1.0f);
    vertices.m_PositionsBuffer.push_back(F);

    glm::vec3 G;
    G.x = min.x;
    G.y = min.y;
    G.z = min.z;
    G = glm::vec4(G, 1.0f);
    vertices.m_PositionsBuffer.push_back(G);

    glm::vec3 H;
    H.x = max.x;
    H.y = min.y;
    H.z = min.z;
    H = glm::vec4(H, 1.0f);
    vertices.m_PositionsBuffer.push_back(H);

    glm::vec3 I;
    I.x = min.x;
    I.y = max.y;
    I.z = min.z;
    I = glm::vec4(I, 1.0f) ;
    vertices.m_PositionsBuffer.push_back(I);

    vertices.min = glm::vec4(min, 1.0f);
    vertices.max = glm::vec4(max, 1.0f);
}