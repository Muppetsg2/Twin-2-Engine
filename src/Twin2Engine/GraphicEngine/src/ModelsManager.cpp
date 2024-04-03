#include "ModelsManager.h"
#include "ModelsManager.h"
#include "ModelsManager.h"
#include "ModelsManager.h"
#include <ModelsManager.h>

#include <spdlog/spdlog.h>
#include <InstatiatingMesh.h>

//using ModelData;

using namespace Twin2Engine::GraphicEngine;

std::hash<std::string> ModelsManager::stringHash;
std::list<ModelData*> ModelsManager::loadedModels;


#if ASSIMP_LOADING

inline void Twin2Engine::GraphicEngine::ModelsManager::LoadModelAssimp(const std::string& modelPath, ModelData* modelData)
{
    // Create an instance of the Assimp importer
    Assimp::Importer importer;

    // Read the file with Assimp
    const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    // Check if the scene was loaded successfully
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        SPDLOG_ERROR("Assimp error: {}", importer.GetErrorString());
        return;
    }

    modelData->meshes.resize(scene->mNumMeshes);

    // Extract mesh data
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        ExtractMeshAssimp(mesh, vertices, indices);

        modelData->meshes[i] = new InstatiatingMesh(vertices, indices);
        //modelData->meshes.push_back(new InstatiatingMesh(vertices, indices));
    }
}

inline void Twin2Engine::GraphicEngine::ModelsManager::ExtractMeshAssimp(const aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    // Reserve memory for vertices
    vertices.resize(mesh->mNumVertices);

    // Extract vertex data
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        // Position
        vertices[i].Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Normal
        if (mesh->HasNormals()) {
            vertices[i].Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        // Texture coordinates
        if (mesh->HasTextureCoords(0)) {
            vertices[i].TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
    }

    // Reserve memory for indices
    indices.reserve(mesh->mNumFaces * 3); // Assuming triangles

    // Extract index data
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
}

#elif TINYGLTF_LOADING

inline void Twin2Engine::GraphicEngine::ModelsManager::LoadModelGLTF(const std::string& modelPath, ModelData* modelData)
{
    // Load GLTF model
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, modelPath);

    if (!warn.empty()) {
        SPDLOG_WARN("Warning: {}", warn);
    }

    if (!err.empty()) {
        SPDLOG_ERROR("Error: {}", err);
        return;
    }

    if (!ret) {
        SPDLOG_ERROR("Failed to load GLTF file");
        return;
    }

    //size_t strHash = stringHash(modelPath);
    //ModelData* modelData = new ModelData{
    //    .modelHash = strHash,
    //    .useNumber = 1
    //};

    modelData->meshes.resize(model.meshes.size());

    // Extract mesh data
    //if (model.meshes.size() > 0) {
    //    const tinygltf::Mesh& mesh = model.meshes[0]; // Assume we're extracting data from the first mesh
    //    std::vector<Vertex> vertices;
    //    std::vector<unsigned int> indices;
    //    ExtractMeshGLTF(mesh, model, vertices, indices);
    //
    //    InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);
    //}

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (int i = 0; i < model.meshes.size(); i++)
    {
        ExtractMeshGLTF(model.meshes[i], model, vertices, indices);

        modelData->meshes[i] = new InstatiatingMesh(vertices, indices);
    }
}

inline void Twin2Engine::GraphicEngine::ModelsManager::ExtractMeshGLTF(const tinygltf::Mesh& mesh, const tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    size_t lastVericesSize = 0;
    size_t lastIndicesSize = 0;
    for (const auto& primitive : mesh.primitives)
    {
        // Extract vertex data

        //Extracting Positions
        const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
        const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
        const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
        const float* positionData = reinterpret_cast<const float*>(&positionBuffer.data[positionAccessor.byteOffset + positionBufferView.byteOffset]);
        const int numComponents = positionAccessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2;
        const int numVertices = positionAccessor.count;

        //Extracting Normals
        const tinygltf::Accessor* normalAccessor = nullptr;
        const float* normalData = nullptr;
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) 
        {
            normalAccessor = &model.accessors[primitive.attributes.at("NORMAL")];
            const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor->bufferView];
            const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
            normalData = reinterpret_cast<const float*>(&normalBuffer.data[normalAccessor->byteOffset + normalBufferView.byteOffset]);
        }

        //Extracting TexCoords
        const tinygltf::Accessor* texCoordAccessor = nullptr;
        const float* texCoordData = nullptr;
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) 
        {
            texCoordAccessor = &model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const tinygltf::BufferView& texCoordBufferView = model.bufferViews[texCoordAccessor->bufferView];
            const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordBufferView.buffer];
            texCoordData = reinterpret_cast<const float*>(&texCoordBuffer.data[texCoordAccessor->byteOffset + texCoordBufferView.byteOffset]);
        }

        // Add vertices to vector
        vertices.resize(lastVericesSize + numVertices);
        for (int k = 0; k < numVertices; ++k) 
        {
            // Position
            vertices[lastVericesSize + k].Position = glm::vec3(positionData[k * numComponents], positionData[k * numComponents + 1], positionData[k * numComponents + 2]);
            // Normal
            if (normalAccessor && normalData) 
            {
                vertices[lastVericesSize + k].Normal = glm::vec3(normalData[k * 3], normalData[k * 3 + 1], normalData[k * 3 + 2]);
            }
            // TexCoords
            if (texCoordAccessor && texCoordData) 
            {
                vertices[lastVericesSize + k].TexCoords = glm::vec2(texCoordData[k * 2], texCoordData[k * 2 + 1]);
            }
        }
        lastVericesSize = vertices.size();

        // Extract index data
        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
        const int numIndices = indexAccessor.count;

        indices.resize(lastIndicesSize + numIndices);

        if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) 
        {
            const uint16_t* indexData = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]);
            for (int k = 0; k < numIndices; ++k) 
            {
                indices[lastIndicesSize + k] = indexData[k];
            }
        }
        else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) 
        {
            const uint32_t* indexData = reinterpret_cast<const uint32_t*>(&indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]);
            for (int k = 0; k < numIndices; ++k) 
            {
                indices[lastIndicesSize + k] = indexData[k];
            }
        }
        else 
        {
            SPDLOG_ERROR("Unsupported index type");
            return;
        }
        lastIndicesSize = indices.size();
    }
}

#endif

Twin2Engine::GraphicEngine::ModelData* Twin2Engine::GraphicEngine::ModelsManager::LoadModel(const std::string& modelPath)
{
    size_t strHash = stringHash(modelPath);

    std::list<ModelData*>::iterator found =
        std::find_if(loadedModels.begin(), loadedModels.end(), [strHash](ModelData* data) { return data->modelHash == strHash; });
    
    ModelData* modelData;
    if (found == loadedModels.end())
    {
        SPDLOG_INFO("Loading model: {}!", modelPath);

        //Model* model = new Model(modelPath.c_str());

        ////Creating SSBO
        //
        //GLuint ssbo;
        //glGenBuffers(1, &ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        modelData = new ModelData{
            .modelHash = strHash,
            .useNumber = 1,
            //.model = model,
            //.ssboId = ssbo
        };

#if ASSIMP_LOADING
        LoadModelAssimp(modelPath, modelData);
#elif TINYGLTF_LOADING
        LoadModelGLTF(modelPath, modelData);
#endif
        //for (Mesh& mesh : model->meshes)
        //{
        //    modelData->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        //}

        loadedModels.push_back(modelData);
    }
    else
    {
        SPDLOG_INFO("Model already loaded: {}!", modelPath);
        modelData = (*found);
        (*found)->useNumber++;
    }

    return modelData;

}

void Twin2Engine::GraphicEngine::ModelsManager::UnloadModel(Twin2Engine::GraphicEngine::ModelData* modelData)
{
    //std::cout << "Tutaj6\n";
    if (modelData != nullptr)
    {
        //std::cout << "Tutaj7\n";
        modelData->useNumber--;
        if (modelData->useNumber == 0)
        {
            //std::cout << "Tutaj8\n";
            std::list<Twin2Engine::GraphicEngine::ModelData*>::iterator found = std::find_if(loadedModels.begin(), loadedModels.end(), [modelData](Twin2Engine::GraphicEngine::ModelData* data) { return data == modelData; });

            if (found != loadedModels.end())
            {
                loadedModels.erase(found);

                //delete (*found)->model;
                for (InstatiatingMesh* mesh : (*found)->meshes)
                {
                    delete mesh;
                }
                delete (*found);
            }
            else
            {

                SPDLOG_ERROR("THIS INSTRUCTION SHOULDNT HAPPEND");
            }
        }
    }
}

void Twin2Engine::GraphicEngine::ModelsManager::Init()
{

}

void Twin2Engine::GraphicEngine::ModelsManager::End()
{

}

Twin2Engine::GraphicEngine::InstatiatingModel Twin2Engine::GraphicEngine::ModelsManager::GetModel(const std::string& modelPath)
{
    ModelData* modelData = LoadModel(modelPath);

    InstatiatingModel model = InstatiatingModel(modelData);

    return model;
}

InstatiatingModel Twin2Engine::GraphicEngine::ModelsManager::CreateModel(const std::string& modelName, vector<Vertex> vertices, vector<unsigned int> indices)
{
    size_t strHash = stringHash(modelName);

    std::list<ModelData*>::iterator found =
        std::find_if(loadedModels.begin(), loadedModels.end(), [strHash](ModelData* data) { return data->modelHash == strHash; });

    ModelData* modelData;
    if (found == loadedModels.end())
    {
        SPDLOG_INFO("Creating model: {}!", modelName);

        InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);

        //Creating SSBO do usuniêcia

        //GLuint ssbo;
        //glGenBuffers(1, &ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        modelData = new ModelData{
            .modelHash = strHash,
            .useNumber = 1,
            //.model = model,
            //.ssboId = ssbo
        };

        modelData->meshes.resize(1);
        modelData->meshes[0] = mesh;

        //for (Mesh& mesh : model->meshes)
        //{
        //    modelData->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        //}

        loadedModels.push_back(modelData);
    }
    else
    {
        SPDLOG_INFO("Model already exist: {}!", modelName);
        modelData = (*found);
        (*found)->useNumber++;
    }

    return InstatiatingModel(modelData);
}

//void GraphicEngine::ModelsManager::FreeModel(InstatiatingModel*& model)
//{
//    model->modelData->useNumber--;
//    model = nullptr;
//}