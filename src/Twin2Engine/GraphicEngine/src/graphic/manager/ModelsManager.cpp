#include <graphic/manager/ModelsManager.h>
#include <regex>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

#if _DEBUG
bool ModelsManager::_fileDialogOpen = false;
ImFileDialogInfo ModelsManager::_fileDialogInfo;
#endif

std::hash<std::string> ModelsManager::_stringHash;
std::map<size_t, ModelData*> ModelsManager::_loadedModels;

std::map<size_t, std::string> ModelsManager::_modelsPaths;

#if ASSIMP_LOADING

inline void ModelsManager::LoadModelAssimp(const std::string& modelPath, ModelData* modelData)
{
    // Create an instance of the Assimp importer
    Assimp::Importer importer = Assimp::Importer();

    // Read the file with Assimp
    const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // Check if the scene was loaded successfully
    if (scene == nullptr) {
        SPDLOG_ERROR("Assimp error: {}", importer.GetErrorString());
        return;
    }
    
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
        SPDLOG_ERROR("Assimp error: {}", importer.GetErrorString());
        return;
    }

    ProcessNodeAssimp(scene->mRootNode, scene, modelData);

    //modelData->meshes.resize(scene->mNumMeshes);
    //if (modelPath == "res/models/castle.obj") SPDLOG_INFO("{}", scene->mNumMeshes);

    /*
    // Extract mesh data
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        //const aiMesh* mesh = scene->mMeshes[i];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        ExtractMeshAssimp(scene->mMeshes[i], &vertices, &indices);

        //modelData->meshes[i] = new InstantiatingMesh(vertices, indices);
        modelData->meshes.emplace_back(new InstantiatingMesh(&vertices, &indices));
        vertices.clear();
        indices.clear();
    }
    */

    //if (modelPath == "res/models/castle.obj") SPDLOG_INFO("{}", modelData->meshes.size());
    importer.FreeScene();
}

inline void ModelsManager::ProcessNodeAssimp(aiNode* node, const aiScene* scene, ModelData* modelData) {
    // process all the node's meshes (if any)

    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<unsigned int> indices = std::vector<unsigned int>();

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        ExtractMeshAssimp(scene->mMeshes[node->mMeshes[i]], vertices, indices);
        modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));

        vertices.clear();
        indices.clear();
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNodeAssimp(node->mChildren[i], scene, modelData);
    }
}

inline void ModelsManager::ExtractMeshAssimp(aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    // Reserve memory for vertices
    //vertices->resize(mesh->mNumVertices);

    bool texCoord = mesh->HasTextureCoords(0);
    bool normal = mesh->HasNormals();
    bool tanBitan = mesh->HasTangentsAndBitangents();
    unsigned int size = mesh->mNumVertices;

    Vertex v;

    // Extract vertex data
    for (unsigned int i = 0; i < size; ++i) {

        v.Position.x = mesh->mVertices[i].x;
        v.Position.y = mesh->mVertices[i].y;
        v.Position.z = mesh->mVertices[i].z;

        if (texCoord) {
            v.TexCoords.x = mesh->mTextureCoords[0][i].x;
            v.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }

        if (normal) {
            v.Normal.x = mesh->mNormals[i].x;
            v.Normal.y = mesh->mNormals[i].y;
            v.Normal.z = mesh->mNormals[i].z;
        }

        if (tanBitan) {
            v.Tangent.x = mesh->mTangents[i].x;
            v.Tangent.y = mesh->mTangents[i].y;
            v.Tangent.z = mesh->mTangents[i].z;
            v.Bitangent.x = mesh->mBitangents[i].x;
            v.Bitangent.y = mesh->mBitangents[i].y;
            v.Bitangent.z = mesh->mBitangents[i].z;
        }

        vertices.push_back(v);
    }

    // Reserve memory for indices
    //indices->reserve(mesh->mNumFaces * 3); // Assuming triangles

    size = mesh->mNumFaces;
    // Extract index data
    for (unsigned int i = 0; i < size; i++)
    {
        aiFace face = mesh->mFaces[i];
        unsigned int size2 = face.mNumIndices;
        for (unsigned int j = 0; j < size2; j++)
            indices.push_back(face.mIndices[j]);
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
    //    InstantiatingMesh* mesh = new InstantiatingMesh(vertices, indices);
    //}

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (int i = 0; i < model.meshes.size(); i++)
    {
        ExtractMeshGLTF(model.meshes[i], model, vertices, indices);

        modelData->meshes[i] = new InstantiatingMesh(vertices, indices);
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

void ModelsManager::LoadCube(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Cube Model");

    std::vector<Vertex> vertices{
        // Front And Bottom Face
        {.Position = glm::vec3(-0.5f, 0.5f, 0.5f),      .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, 0.0f, 1.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, 0.5f, 0.5f),       .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, 0.0f, 1.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, 0.5f, -0.5f),      .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, 0.0f, -1.0f), .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(-0.5f, 0.5f, -0.5f),     .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, 0.0f, -1.0f), .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(-0.5f, -0.5f, 0.5f),     .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.0f, 1.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, 0.5f),      .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.0f, 1.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, -0.5f),     .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.0f, -1.0f), .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(-0.5f, -0.5f, -0.5f),    .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.0f, -1.0f), .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },

        // Right And Left Face
        {.Position = glm::vec3(-0.5f, 0.5f, 0.5f),      .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(-1.0f, 0.0f, 0.0f), .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, 0.5f, 0.5f),       .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f),  .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, 0.5f, -0.5f),      .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f),  .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(-0.5f, 0.5f, -0.5f),     .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(-1.0f, 0.0f, 0.0f), .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(-0.5f, -0.5f, 0.5f),     .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(-1.0f, 0.0f, 0.0f), .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, 0.5f),      .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f),  .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, -0.5f),     .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f),  .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },
        {.Position = glm::vec3(-0.5f, -0.5f, -0.5f),    .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(-1.0f, 0.0f, 0.0f), .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),     .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f) },

        // Top And Bottom Face
        {.Position = glm::vec3(-0.5f, 0.5f, 0.5f),      .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)  },
        {.Position = glm::vec3(0.5f, 0.5f, 0.5f),       .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)  },
        {.Position = glm::vec3(0.5f, 0.5f, -0.5f),      .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)  },
        {.Position = glm::vec3(-0.5f, 0.5f, -0.5f),     .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f),  .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)  },
        {.Position = glm::vec3(-0.5f, -0.5f, 0.5f),     .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, 0.5f),      .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, -0.5f),     .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f) },
        {.Position = glm::vec3(-0.5f, -0.5f, -0.5f),    .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f) }
    };

    std::vector<unsigned int> indices = {
        // Front Face
        0, 4, 5,
        0, 5, 1,

        // Back Face
        2, 6, 7,
        2, 7, 3,

        // Right Face
        9, 13, 14,
        9, 14, 10,

        // Left Face
        11, 15, 12,
        11, 12, 8,

        // Top Face
        19, 16, 17,
        19, 17, 18,

        // Bottom Face
        20, 23, 22,
        20, 22, 21
    };

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadPlane(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Plane Model");

    std::vector<Vertex> vertices{
        {.Position = glm::vec3(-0.5f, 0.0f, -0.5f),    .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f), .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f) },
        {.Position = glm::vec3(0.5f, 0.0f, -0.5f),     .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f), .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f) },
        {.Position = glm::vec3(-0.5f, 0.0f, 0.5f),     .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f), .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f) },
        {.Position = glm::vec3(0.5f, 0.0f, 0.5f),      .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, 1.0f, 0.0f), .Tangent = glm::vec3(1.0f, 0.0f, 0.0f), .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f) }
    };

    std::vector<unsigned int> indices = {
        2, 1, 0,
        2, 3, 1
    };

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadSphere(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Sphere Model");

    uint32_t horizontalSegments = 12;
    uint32_t verticalSegments = 12;

    float angleYDiff = 180.f / (float)horizontalSegments;
    float angleXZDiff = 360.f / (float)verticalSegments;

    float texHDiff = 1.f / (float)horizontalSegments;
    float texVDiff = 1.f / (float)verticalSegments;

    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<unsigned int> indices = std::vector<unsigned int>();
    std::vector<unsigned int> trisNum = std::vector<unsigned int>();

    // VERTICIES AND NUMBER OF TRIANGLES
    // TOP VERTEX
    vertices.push_back({ { 0.f, 1.f, 0.f }, { .5f, 0.f }, { 0.f, 1.f, 0.f }, glm::vec3(0.f), glm::vec3(0.f) });
    trisNum.push_back(verticalSegments);
    // TOP HALF AND BOTTOM HALF
    float angleY = angleYDiff;
    for (unsigned int i = 0; i < horizontalSegments - 1; ++i) {
        float radiansY = glm::radians(angleY);
        float r = sinf(radiansY);
        float y = cosf(radiansY);

        unsigned int startTexV = i * verticalSegments + 1;
        unsigned int t = 1;

        if (horizontalSegments == 2) {
            t = 4;
        }
        else if (horizontalSegments == 3) {
            t = 5;
        }
        else if (horizontalSegments >= 4) {
            if (i == 0 || i + 2 == horizontalSegments) {
                t = 5;
            }
            else {
                t = 6;
            }
        }

        // DRAW CIRCLE
        float angleXZ = 0.f;
        for (unsigned int j = 0; j < verticalSegments; ++j) {
            float radiansXZ = glm::radians(angleXZ);
            float z = r * cosf(radiansXZ);
            float x = r * sinf(radiansXZ);

            glm::vec3 vert = { x, y, z };
            vertices.push_back({ vert, { j * texVDiff, texHDiff * (i + 1) }, glm::normalize(vert), glm::vec3(0.f), glm::vec3(0.f) });
            trisNum.push_back(t);

            if (j == verticalSegments - 1)
            {
                glm::vec3 vertLast = { r * sinf(glm::radians(0.f)), y, r * cosf(glm::radians(0.f)) };
                // Add first in the end again for texCoords
                vertices.push_back({ vertLast, { 1.f , texHDiff * (i + 1) }, glm::normalize(vertLast), glm::vec3(0.f), glm::vec3(0.f) });
                trisNum.push_back(t);
            }

            angleXZ += angleXZDiff;
        }
        angleY += angleYDiff;
    }
    // BOTTOM VERTEX
    vertices.push_back({ { 0.f, -1.f, 0.f }, { .5f, 1.f }, { 0.f, -1.f, 0.f }, glm::vec3(0.f), glm::vec3(0.f) });
    trisNum.push_back(verticalSegments);

    // INDICIES, TANGENTS AND BITANGENTS
    std::pair<glm::vec3, glm::vec3> TB;

    size_t verticesNum = vertices.size();
    // TOP CIRCLE + BOTTOM CIRCLE
    for (unsigned int i = 0; i < verticalSegments; ++i) {
        // TOP CIRCLE
        unsigned int rightVertex = (i + 1) + 1;
        unsigned int topVertex = 0;
        unsigned int leftVertex = i + 1;

        indices.push_back(rightVertex);
        indices.push_back(topVertex);
        indices.push_back(leftVertex);

        TB = CalcTangentBitangent(vertices, rightVertex, topVertex, leftVertex);

        vertices[rightVertex].Tangent += TB.first;
        vertices[rightVertex].Bitangent += TB.second;

        vertices[topVertex].Tangent += TB.first;
        vertices[topVertex].Bitangent += TB.second;

        vertices[leftVertex].Tangent += TB.first;
        vertices[leftVertex].Bitangent += TB.second;

        // BOTTOM CIRCLE
        rightVertex = verticesNum - 2 - verticalSegments - 1 + (i + 1) + 1;
        leftVertex = verticesNum - 2 - verticalSegments - 1 + i + 1;
        topVertex = verticesNum - 1;

        indices.push_back(rightVertex);
        indices.push_back(leftVertex);
        indices.push_back(topVertex);

        TB = CalcTangentBitangent(vertices, rightVertex, leftVertex, topVertex);

        vertices[rightVertex].Tangent += TB.first;
        vertices[rightVertex].Bitangent += TB.second;

        vertices[leftVertex].Tangent += TB.first;
        vertices[leftVertex].Bitangent += TB.second;

        vertices[topVertex].Tangent += TB.first;
        vertices[topVertex].Bitangent += TB.second;
    }

    // CENTER CIRCLES
    for (unsigned int c = 0; c < horizontalSegments - 2; ++c) {
        unsigned int startV = c * (verticalSegments + 1) + 1;
        for (unsigned int i = 0; i < verticalSegments; ++i) {
            unsigned int topLeft = i + startV;
            unsigned int topRight = (i + 1) + startV;
            unsigned int bottomLeft = i + verticalSegments + 1 + startV;
            unsigned int bottomRight = (i + 1) + verticalSegments + 1 + startV;

            indices.push_back(topRight);
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);

            TB = CalcTangentBitangent(vertices, topRight, topLeft, bottomLeft);

            vertices[topRight].Tangent += TB.first;
            vertices[topRight].Bitangent += TB.second;

            vertices[topLeft].Tangent += TB.first;
            vertices[topLeft].Bitangent += TB.second;

            vertices[bottomLeft].Tangent += TB.first;
            vertices[bottomLeft].Bitangent += TB.second;

            indices.push_back(bottomRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);

            TB = CalcTangentBitangent(vertices, bottomRight, topRight, bottomLeft);

            vertices[bottomRight].Tangent += TB.first;
            vertices[bottomRight].Bitangent += TB.second;

            vertices[topRight].Tangent += TB.first;
            vertices[topRight].Bitangent += TB.second;

            vertices[bottomLeft].Tangent += TB.first;
            vertices[bottomLeft].Bitangent += TB.second;
        }
    }

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        vertices[i].Tangent /= (float)trisNum[i];
        vertices[i].Tangent = glm::normalize(vertices[i].Tangent);

        vertices[i].Bitangent /= (float)trisNum[i];
        vertices[i].Bitangent = glm::normalize(vertices[i].Bitangent);
    }

    trisNum.clear();

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadTorus(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Torus Model");

    unsigned int segments = 12;	    // co-planar circular axis resolution
    unsigned int cs_segments = 12;  // revolving circle resolution
    float radius = .75f;            // co-planar circular axis radius
    float cs_radius = .25f;         // revolving circle radius

    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<unsigned int> indices = std::vector<unsigned int>();

    float angleincs = 360.f / (float)segments;
    float cs_angleincs = 360.f / (float)cs_segments;
    float currentradius, yval;
    int i, j, nextrow;

    /* iterate cs_sides: inner ring */
    for (j = 0; j <= cs_segments; ++j)
    {
        float radJ = glm::radians((float)j * cs_angleincs);
        currentradius = radius + (cs_radius * cosf(radJ));
        yval = cs_radius * sinf(radJ);

        /* iterate sides: outer ring */
        for (i = 0; i <= segments; ++i)
        {
            float radI = glm::radians((float)i * angleincs);

            float u = ((float)i * angleincs) / 360.f;
            float v = ((2.f * (float)j * cs_angleincs) / 360.f) - 1.f;
            if (v < 0.f) v = -v;

            float xc = radius * cosf(radI);
            float zc = radius * sinf(radI);

            glm::vec3 pos = glm::vec3(currentradius * cosf(radI), yval, currentradius * sinf(radI));

            vertices.push_back({ pos, { u, v }, glm::normalize(glm::vec3(pos.x - xc, pos.y, pos.z - zc)), glm::vec3(0.f), glm::vec3(0.f) });
        }
    }

    /* inner ring */
    for (i = 0, nextrow = segments + 1; i < cs_segments; i++)
    {
        // outer ring
        for (j = 0; j < segments; j++)
        {
            int first = i * nextrow + j;
            int second = i * nextrow + j + 1;
            int third = i * nextrow + j + nextrow;
            int fourth = i * nextrow + j + nextrow + 1;

            indices.push_back(third);
            indices.push_back(second);
            indices.push_back(first);

            std::pair<glm::vec3, glm::vec3> TB = CalcTangentBitangent(vertices, third, second, first);

            vertices[third].Tangent += TB.first;
            vertices[third].Bitangent += TB.second;

            vertices[second].Tangent += TB.first;
            vertices[second].Bitangent += TB.second;

            vertices[first].Tangent += TB.first;
            vertices[first].Bitangent += TB.second;

            indices.push_back(second);
            indices.push_back(third);
            indices.push_back(fourth);

            TB = CalcTangentBitangent(vertices, second, third, fourth);

            vertices[second].Tangent += TB.first;
            vertices[second].Bitangent += TB.second;

            vertices[third].Tangent += TB.first;
            vertices[third].Bitangent += TB.second;

            vertices[fourth].Tangent += TB.first;
            vertices[fourth].Bitangent += TB.second;
        }
    }

    std::map<size_t, unsigned int> trisNum;

    for (unsigned int i : indices) {
        trisNum[i] += 1;
    }

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        vertices[i].Tangent /= (float)trisNum[i];
        vertices[i].Tangent = glm::normalize(vertices[i].Tangent);

        vertices[i].Bitangent /= (float)trisNum[i];
        vertices[i].Bitangent = glm::normalize(vertices[i].Bitangent);
    }

    trisNum.clear();

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadCone(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Cone Model");

    unsigned int segments = 12;
    float h = 1.f;
    float r = 1.f;

    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<unsigned int> indices = std::vector<unsigned int>();

    std::vector<unsigned int> trisNum;

    GenerateCircle(vertices, indices, segments, -h / 2.f, GL_CW);

    // CONE
    // VERTICES AND TEX COORDS
    size_t start = vertices.size();
    float angleXZDiff = 360.f / (float)segments;
    float y = -h / 2.f;
    float angleXZ = 0.f;
    float cos_cone = r / sqrtf(r * r + h * h);
    float sin_cone = h / sqrtf(r * r + h * h);
    for (unsigned int j = 0; j < segments; ++j) {
        float radiansXZ = glm::radians(angleXZ);
        float z = cosf(radiansXZ);
        float x = sinf(radiansXZ);

        glm::vec3 norm = glm::normalize(glm::vec3(cos_cone * x, sin_cone, cos_cone * z));

        vertices.push_back({ { x * r, y, z * r }, { angleXZ / 360.f, 1.f }, norm, glm::vec3(0.f), glm::vec3(0.f) });

        if (j == 0) {
            trisNum.push_back(1);
        }
        else {
            trisNum.push_back(2);
        }

        angleXZ += angleXZDiff;
    }

    vertices.push_back({ { 0.f, y, r }, { 1.f, 1.f }, { 0.f, sin_cone, cos_cone }, glm::vec3(0.f), glm::vec3(0.f) });
    trisNum.push_back(1);
    vertices.push_back({ { 0.f, -y, 0.f }, { .5f, 0.f }, { 0.f, 1.f, 0.f }, glm::vec3(0.f), glm::vec3(0.f) });
    trisNum.push_back(segments);

    // INDICES
    for (unsigned int i = 0; i < segments; ++i) {

        int left = start + i;
        int right = start + i + 1;
        int top = vertices.size() - 1;

        indices.push_back(left);
        indices.push_back(right);
        indices.push_back(top);

        std::pair<glm::vec3, glm::vec3> TB = CalcTangentBitangent(vertices, left, right, top);

        vertices[left].Tangent += TB.first;
        vertices[left].Bitangent += TB.second;

        vertices[right].Tangent += TB.first;
        vertices[right].Bitangent += TB.second;

        vertices[top].Tangent += TB.first;
        vertices[top].Bitangent += TB.second;
    }

    for (unsigned int i = start; i < vertices.size(); ++i) {
        vertices[i].Tangent /= (float)trisNum[i - start];
        vertices[i].Tangent = glm::normalize(vertices[i].Tangent);

        vertices[i].Bitangent /= (float)trisNum[i - start];
        vertices[i].Bitangent = glm::normalize(vertices[i].Bitangent);
    }

    trisNum.clear();

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadPiramid(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Piramid Model");

    std::vector<Vertex> vertices{
        { .Position = glm::vec3(-0.5f, -0.353553f, 0.5f),   .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f),             .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f)           },
        { .Position = glm::vec3(0.5f, -0.353553f, 0.5f),    .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f),             .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f)           },
        { .Position = glm::vec3(-0.5f, -0.353553f, -0.5f),  .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f),             .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f)           },
        { .Position = glm::vec3(0.5f, -0.353553f, -0.5f),   .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f),             .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, 0.0f, -1.0f)           },
        
        { .Position = glm::vec3(-0.5f, -0.353553f, 0.5f),   .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.447214f, 0.894427f),    .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, -0.816497f, 0.57735f)  },
        { .Position = glm::vec3(0.5f, -0.353553f, 0.5f),    .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.447214f, 0.894427f),    .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, -0.816497f, 0.57735f)  },
        { .Position = glm::vec3(0.0f, 0.353553f, 0.0f),     .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(0.0f, 0.447214f, 0.894427f),    .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),     .Bitangent = glm::vec3(0.0f, -0.816497f, 0.57735f)  },
        
        { .Position = glm::vec3(0.5f, -0.353553f, 0.5f),    .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.894427f, 0.447214f, 0.0f),    .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),    .Bitangent = glm::vec3(0.57735f, -0.816497f, 0.0f)  },
        { .Position = glm::vec3(0.5f, -0.353553f, -0.5f),   .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.894427f, 0.447214f, 0.0f),    .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),    .Bitangent = glm::vec3(0.57735f, -0.816497f, 0.0f)  },
        { .Position = glm::vec3(0.0f, 0.353553f, 0.0f),     .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(0.894427f, 0.447214f, 0.0f),    .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),    .Bitangent = glm::vec3(0.57735f, -0.816497f, 0.0f)  },
        
        { .Position = glm::vec3(0.5f, -0.353553f, -0.5f),   .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.447214f, -0.894427f),   .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),    .Bitangent = glm::vec3(0.0f, -0.816497f, -0.57735f) },
        { .Position = glm::vec3(-0.5f, -0.353553f, -0.5f),  .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.447214f, -0.894427f),   .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),    .Bitangent = glm::vec3(0.0f, -0.816497f, -0.57735f) },
        { .Position = glm::vec3(0.0f, 0.353553f, 0.0f),     .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(0.0f, 0.447214f, -0.894427f),   .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),    .Bitangent = glm::vec3(0.0f, -0.816497f, -0.57735f) },
        
        { .Position = glm::vec3(-0.5f, -0.353553f, -0.5f),  .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(-0.894427f, 0.447214f, 0.0f),   .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),     .Bitangent = glm::vec3(-0.57735f, -0.816497f, 0.0f) },
        { .Position = glm::vec3(-0.5f, -0.353553f, 0.5f),   .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(-0.894427f, 0.447214f, 0.0f),   .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),     .Bitangent = glm::vec3(-0.57735f, -0.816497f, 0.0f) },
        { .Position = glm::vec3(0.0f, 0.353553f, 0.0f),     .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(-0.894427f, 0.447214f, 0.0f),   .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),     .Bitangent = glm::vec3(-0.57735f, -0.816497f, 0.0f) }
    };

    std::vector<unsigned int> indices{
        0, 2, 1,
        1, 2, 3,

        4, 5, 6,

        7, 8, 9,

        10, 11, 12,

        13, 14, 15
    };

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadTetrahedron(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Tetrahedron Model");

    std::vector<Vertex> vertices{
        { .Position = glm::vec3(0.0f, -0.333333f, 0.577350f),   .TexCoords = glm::vec2(0.5f, 1.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f),                 .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),        .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)                    },
        { .Position = glm::vec3(0.5f, -0.333333f, -0.288675f),  .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f),                 .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),        .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)                    },
        { .Position = glm::vec3(-0.5f, -0.333333f, -0.288675f), .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f, 0.0f),                 .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),        .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)                    },
        
        { .Position = glm::vec3(0.0f, -0.333333f, 0.577350f),   .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.344124f, 0.917663f, 0.19868f),    .Tangent = glm::vec3(0.5f, 0.0f, -0.866025f),   .Bitangent = glm::vec3(0.344124f, -0.917663f, 0.19868f)     },
        { .Position = glm::vec3(0.5f, -0.333333f, -0.288675f),  .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.344124f, 0.917663f, 0.19868f),    .Tangent = glm::vec3(0.5f, 0.0f, -0.866025f),   .Bitangent = glm::vec3(0.344124f, -0.917663f, 0.19868f)     },
        { .Position = glm::vec3(0.0f, 0.333333f, 0.0f),         .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(0.344124f, 0.917663f, 0.19868f),    .Tangent = glm::vec3(0.5f, 0.0f, -0.866025f),   .Bitangent = glm::vec3(0.344124f, -0.917663f, 0.19868f)     },
        
        { .Position = glm::vec3(0.5f, -0.333333f, -0.288675f),  .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.917663f, -0.39736f),        .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),        .Bitangent = glm::vec3(0.0f, -0.917663f, -0.39736f)         },
        { .Position = glm::vec3(-0.5f, -0.333333f, -0.288675f), .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, 0.917663f, -0.39736f),        .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),        .Bitangent = glm::vec3(0.0f, -0.917663f, -0.39736f)         },
        { .Position = glm::vec3(0.0f, 0.333333f, 0.0f),         .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(0.0f, 0.917663f, -0.39736f),        .Tangent = glm::vec3(-1.0f, 0.0f, 0.0f),        .Bitangent = glm::vec3(0.0f, -0.917663f, -0.39736f)         },
        
        { .Position = glm::vec3(-0.5f, -0.333333f, -0.288675f), .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(-0.344124f, 0.917663f, 0.19868f),   .Tangent = glm::vec3(0.5f, 0.0f, 0.866025f),    .Bitangent = glm::vec3(-0.344124f, -0.917663f, 0.19868f)    },
        { .Position = glm::vec3(0.0f, -0.333333f, 0.577350f),   .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(-0.344124f, 0.917663f, 0.19868f),   .Tangent = glm::vec3(0.5f, 0.0f, 0.866025f),    .Bitangent = glm::vec3(-0.344124f, -0.917663f, 0.19868f)    },
        { .Position = glm::vec3(0.0f, 0.333333f, 0.0f),         .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(-0.344124f, 0.917663f, 0.19868f),   .Tangent = glm::vec3(0.5f, 0.0f, 0.866025f),    .Bitangent = glm::vec3(-0.344124f, -0.917663f, 0.19868f)    }
    };

    std::vector<unsigned int> indices{
        0, 2, 1,

        3, 4, 5,

        6, 7, 8,

        9, 10, 11
    };

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadCylinder(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Cylinder Model");

    unsigned int segments = 12;
    float h = 1.f;

    std::vector<Vertex> vertices = std::vector<Vertex>();   
    std::vector<unsigned int> indices = std::vector<unsigned int>();

    GenerateCircle(vertices, indices, segments, h / 2.f, GL_CCW);

    std::vector<unsigned int> trisNum;

    float angleXZDiff = 360.f / (float)segments;

    unsigned int start = vertices.size();

    // VERTICES UP AND DOWN
    for (int i = 0; i < 2; ++i) {
        float y = h / 2.f - h * i;
        float angleXZ = 0.f;
        for (unsigned int j = 0; j < segments; ++j) {
            float radiansXZ = glm::radians(angleXZ);
            float z = cosf(radiansXZ);
            float x = sinf(radiansXZ);
            vertices.push_back({ { x, y, z }, { angleXZ / 360.f, (float)i }, glm::normalize(glm::vec3(x, 0.f, z)), glm::vec3(0.f), glm::vec3(0.f) });

            if (j == 0) {
                trisNum.push_back(1 + i);
            }
            else {
                trisNum.push_back(3);
            }
            angleXZ += angleXZDiff;
        }

        vertices.push_back({ { 0.f, y, 1.f }, { 1.f, (float)i }, { 0.f, 0.f, 1.f }, glm::vec3(0.f), glm::vec3(0.f) });
        trisNum.push_back(2 - i);
    }

    // INDICES
    for (unsigned int i = start; i < start + segments; ++i) {

        indices.push_back(i);
        indices.push_back(i + segments + 1);
        indices.push_back(i + 1);

        std::pair<glm::vec3, glm::vec3> TB = CalcTangentBitangent(vertices, i, i + segments + 1, i + 1);

        vertices[i].Tangent += TB.first;
        vertices[i].Bitangent += TB.second;

        vertices[i + segments + 1].Tangent += TB.first;
        vertices[i + segments + 1].Bitangent += TB.second;

        vertices[i + 1].Tangent += TB.first;
        vertices[i + 1].Bitangent += TB.second;

        indices.push_back(i + 1);
        indices.push_back(i + segments + 1);
        indices.push_back(i + segments + 2);

        TB = CalcTangentBitangent(vertices, i + 1, i + segments + 1, i + segments + 2);

        vertices[i + 1].Tangent += TB.first;
        vertices[i + 1].Bitangent += TB.second;

        vertices[i + segments + 1].Tangent += TB.first;
        vertices[i + segments + 1].Bitangent += TB.second;

        vertices[i + segments + 2].Tangent += TB.first;
        vertices[i + segments + 2].Bitangent += TB.second;
    }

    for (unsigned int i = start; i < vertices.size(); ++i) {
        vertices[i].Tangent /= (float)trisNum[i - start];
        vertices[i].Tangent = glm::normalize(vertices[i].Tangent);

        vertices[i].Bitangent /= (float)trisNum[i - start];
        vertices[i].Bitangent = glm::normalize(vertices[i].Bitangent);
    }

    trisNum.clear();

    GenerateCircle(vertices, indices, segments, -h / 2.f, GL_CW);

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

void ModelsManager::LoadHexagon(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Hexagon Model");

    std::vector<Vertex> vertices {
        { .Position = glm::vec3(0.0f, 0.5f, 1.0f),          .TexCoords = glm::vec2(0.5f, 1.0f),         .Normal = glm::vec3(0.0f, 1.0f, 0.0f),          .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.866025f, 0.5f, 0.5f),     .TexCoords = glm::vec2(0.933013f, 0.75f),   .Normal = glm::vec3(0.0f, 1.0f, 0.0f),          .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.866025f, 0.5f, -0.5f),    .TexCoords = glm::vec2(0.933013f, 0.25f),   .Normal = glm::vec3(0.0f, 1.0f, 0.0f),          .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.0f, 0.5f, -1.0f),         .TexCoords = glm::vec2(0.5f, 0.0f),         .Normal = glm::vec3(0.0f, 1.0f, 0.0f),          .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(-0.866025f, 0.5f, -0.5f),   .TexCoords = glm::vec2(0.066987f, 0.25f),   .Normal = glm::vec3(0.0f, 1.0f, 0.0f),          .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(-0.866025f, 0.5f, 0.5f),    .TexCoords = glm::vec2(0.066987f, 0.75f),   .Normal = glm::vec3(0.0f, 1.0f, 0.0f),          .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.0f, 0.5f, 0.0f),          .TexCoords = glm::vec2(0.5f, 0.5f),         .Normal = glm::vec3(0.0f, 1.0f, 0.0f),          .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        
        { .Position = glm::vec3(0.0f, 0.5f, 1.0f),          .TexCoords = glm::vec2(0.0f, 0.0f),         .Normal = glm::vec3(0.5f, 0.0f, 0.866025f),     .Tangent = glm::vec3(0.866025f, 0.0f, -0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.866025f, 0.5f, 0.5f),     .TexCoords = glm::vec2(1.0f, 0.0f),         .Normal = glm::vec3(0.5f, 0.0f, 0.866025f),     .Tangent = glm::vec3(0.866025f, 0.0f, -0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.866025f, 0.5f, 0.5f),     .TexCoords = glm::vec2(0.0f, 0.0f),         .Normal = glm::vec3(1.0f, 0.0f, 0.0f),          .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),        .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.866025f, 0.5f, -0.5f),    .TexCoords = glm::vec2(1.0f, 0.0f),         .Normal = glm::vec3(1.0f, 0.0f, 0.0f),          .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),        .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        
        { .Position = glm::vec3(0.866025f, 0.5f, -0.5f),    .TexCoords = glm::vec2(0.0f, 0.0f),         .Normal = glm::vec3(0.5f, 0.0f, -0.866025f),    .Tangent = glm::vec3(-0.866025f, 0.0f, -0.5f),  .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.0f, 0.5f, -1.0f),         .TexCoords = glm::vec2(1.0f, 0.0f),         .Normal = glm::vec3(0.5f, 0.0f, -0.866025f),    .Tangent = glm::vec3(-0.866025f, 0.0f, -0.5f),  .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.0f, 0.5f, -1.0f),         .TexCoords = glm::vec2(0.0f, 0.0f),         .Normal = glm::vec3(-0.5f, 0.0f, -0.866025f),   .Tangent = glm::vec3(-0.866025f, 0.0f, 0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(-0.866025f, 0.5f, -0.5f),   .TexCoords = glm::vec2(1.0f, 0.0f),         .Normal = glm::vec3(-0.5f, 0.0f, -0.866025f),   .Tangent = glm::vec3(-0.866025f, 0.0f, 0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        
        { .Position = glm::vec3(-0.866025f, 0.5f, -0.5f),   .TexCoords = glm::vec2(0.0f, 0.0f),         .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),         .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),         .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(-0.866025f, 0.5f, 0.5f),    .TexCoords = glm::vec2(1.0f, 0.0f),         .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),         .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),         .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(-0.866025f, 0.5f, 0.5f),    .TexCoords = glm::vec2(0.0f, 0.0f),         .Normal = glm::vec3(-0.5f, 0.0f, 0.866025f),    .Tangent = glm::vec3(0.866025f, 0.0f, 0.5f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.0f, 0.5f, 1.0f),          .TexCoords = glm::vec2(1.0f, 0.0f),         .Normal = glm::vec3(-0.5f, 0.0f, 0.866025f),    .Tangent = glm::vec3(0.866025f, 0.0f, 0.5f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        
        { .Position = glm::vec3(0.0f, -0.5f, 1.0f),         .TexCoords = glm::vec2(0.0f, 1.0f),         .Normal = glm::vec3(0.5f, 0.0f, 0.866025f),     .Tangent = glm::vec3(0.866025f, 0.0f, -0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.866025f, -0.5f, 0.5f),    .TexCoords = glm::vec2(1.0f, 1.0f),         .Normal = glm::vec3(0.5f, 0.0f, 0.866025f),     .Tangent = glm::vec3(0.866025f, 0.0f, -0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.866025f, -0.5f, 0.5f),    .TexCoords = glm::vec2(0.0f, 1.0f),         .Normal = glm::vec3(1.0f, 0.0f, 0.0f),          .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),        .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.866025f, -0.5f, -0.5f),   .TexCoords = glm::vec2(1.0f, 1.0f),         .Normal = glm::vec3(1.0f, 0.0f, 0.0f),          .Tangent = glm::vec3(0.0f, 0.0f, -1.0f),        .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        
        { .Position = glm::vec3(0.866025f, -0.5f, -0.5f),   .TexCoords = glm::vec2(0.0f, 1.0f),         .Normal = glm::vec3(0.5f, 0.0f, -0.866025f),    .Tangent = glm::vec3(-0.866025f, 0.0f, -0.5f),  .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.0f, -0.5f, -1.0f),        .TexCoords = glm::vec2(1.0f, 1.0f),         .Normal = glm::vec3(0.5f, 0.0f, -0.866025f),    .Tangent = glm::vec3(-0.866025f, 0.0f, -0.5f),  .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.0f, -0.5f, -1.0f),        .TexCoords = glm::vec2(0.0f, 1.0f),         .Normal = glm::vec3(-0.5f, 0.0f, -0.866025f),   .Tangent = glm::vec3(-0.866025f, 0.0f, 0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(-0.866025f, -0.5f, -0.5f),  .TexCoords = glm::vec2(1.0f, 1.0f),         .Normal = glm::vec3(-0.5f, 0.0f, -0.866025f),   .Tangent = glm::vec3(-0.866025f, 0.0f, 0.5f),   .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        
        { .Position = glm::vec3(-0.866025f, -0.5f, -0.5f),  .TexCoords = glm::vec2(0.0f, 1.0f),         .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),         .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),         .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(-0.866025f, -0.5f, 0.5f),   .TexCoords = glm::vec2(1.0f, 1.0f),         .Normal = glm::vec3(-1.0f, 0.0f, 0.0f),         .Tangent = glm::vec3(0.0f, 0.0f, 1.0f),         .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(-0.866025f, -0.5f, 0.5f),   .TexCoords = glm::vec2(0.0f, 1.0f),         .Normal = glm::vec3(-0.5f, 0.0f, 0.866025f),    .Tangent = glm::vec3(0.866025f, 0.0f, 0.5f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        { .Position = glm::vec3(0.0f, -0.5f, 1.0f),         .TexCoords = glm::vec2(1.0f, 1.0f),         .Normal = glm::vec3(-0.5f, 0.0f, 0.866025f),    .Tangent = glm::vec3(0.866025f, 0.0f, 0.5f),    .Bitangent = glm::vec3(0.0f, -1.0f, 0.0f)   },
        
        { .Position = glm::vec3(0.0f, -0.5f, 1.0f),         .TexCoords = glm::vec2(0.5f, 1.0f),         .Normal = glm::vec3(0.0f, -1.0f, 0.0f),         .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.866025f, -0.5f, 0.5f),    .TexCoords = glm::vec2(0.933013f, 0.75f),   .Normal = glm::vec3(0.0f, -1.0f, 0.0f),         .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.866025f, -0.5f, -0.5f),   .TexCoords = glm::vec2(0.933013f, 0.25f),   .Normal = glm::vec3(0.0f, -1.0f, 0.0f),         .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.0f, -0.5f, -1.0f),        .TexCoords = glm::vec2(0.5f, 0.0f),         .Normal = glm::vec3(0.0f, -1.0f, 0.0f),         .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(-0.866025f, -0.5f, -0.5f),  .TexCoords = glm::vec2(0.066987f, 0.25f),   .Normal = glm::vec3(0.0f, -1.0f, 0.0f),         .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(-0.866025f, -0.5f, 0.5f),   .TexCoords = glm::vec2(0.066987f, 0.75f),   .Normal = glm::vec3(0.0f, -1.0f, 0.0f),         .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    },
        { .Position = glm::vec3(0.0f, -0.5f, 0.0f),         .TexCoords = glm::vec2(0.5f, 0.5f),         .Normal = glm::vec3(0.0f, -1.0f, 0.0f),         .Tangent = glm::vec3(1.0f, 0.0f, 0.0f),         .Bitangent = glm::vec3(0.0f, 0.0f, 1.0f)    }
    };

    std::vector<unsigned int> indices{
        0, 1, 6,
        1, 2, 6,
        2, 3, 6,
        3, 4, 6,
        4, 5, 6,
        5, 0, 6,

        7, 19, 8,
        8, 19, 20,
        
        9, 21, 10,
        10, 21, 22,
        
        11, 23, 12,
        12, 23, 24,
        
        13, 25, 14,
        14, 25, 26,
        
        15, 27, 16,
        16, 27, 28,
        
        17, 29, 18,
        18, 29, 30,

        32, 31, 37,
        33, 32, 37,
        34, 33, 37,
        35, 34, 37,
        36, 35, 37,
        31, 36, 37
    };

    modelData->meshes.emplace_back(new InstantiatingMesh(vertices, indices));
}

ModelData* ModelsManager::LoadModelData(const std::string& modelPath)
{
    size_t strHash = _stringHash(modelPath);

    if (!_loadedModels.contains(strHash))
    {
        SPDLOG_INFO("Loading model: {}!", modelPath);

        ModelData* modelData = new ModelData{
            .id = strHash,
        };

        if (modelPath == CUBE_PATH) {
            LoadCube(modelData);
        }
        else if (modelPath == PLANE_PATH) {
            LoadPlane(modelData);
        }
        else if (modelPath == SPHERE_PATH) {
            LoadSphere(modelData);
        }
        else if (modelPath == TORUS_PATH) {
            LoadTorus(modelData);
        }
        else if (modelPath == CONE_PATH) {
            LoadCone(modelData);
        }
        else if (modelPath == PIRAMID_PATH) {
            LoadPiramid(modelData);
        }
        else if (modelPath == TETRAHEDRON_PATH) {
            LoadTetrahedron(modelData);
        }
        else if (modelPath == CYLINDER_PATH) {
            LoadCylinder(modelData);
        }
        else if (modelPath == HEXAGON_PATH) {
            LoadHexagon(modelData);
        }
        else {
            if (std::filesystem::exists(modelPath))
            {
#if ASSIMP_LOADING
                LoadModelAssimp(modelPath, modelData);
#elif TINYGLTF_LOADING
                LoadModelGLTF(modelPath, modelData);
#endif
            }
            else
            {
                SPDLOG_ERROR("Model file '{0}' not found!", modelPath);
                delete modelData;
                return nullptr;
            }
        }

        _loadedModels[strHash] = modelData;
        _modelsPaths[strHash] = modelPath;
    }
    else
    {
        SPDLOG_INFO("Model already loaded: {}!", modelPath);
    }

    return _loadedModels[strHash];
}

void ModelsManager::GenerateCircle(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int segments, float y, unsigned int cullFace)
{
    std::vector<unsigned int> trisNum;

    float angleXZDiff = 360.f / (float)segments;

    // CIRCLE TOP
    // VERTICES AND TEX COORDS
    unsigned int start = vertices.size();

    float angleXZ = 0.f;
    for (unsigned int j = 0; j < segments; ++j) {
        float radiansXZ = glm::radians(angleXZ);
        float z = cosf(radiansXZ);
        float x = sinf(radiansXZ);
        vertices.push_back({ { x, y, z }, { .5f + x * .5f, .5f + z * .5f }, (cullFace == GL_CCW ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(0.f, -1.f, 0.f)), glm::vec3(0.f), glm::vec3(0.f) });
        trisNum.push_back(2);
        angleXZ += angleXZDiff;
    }
    vertices.push_back({ { 0.f, y, 0.f }, { .5f, .5f }, (cullFace == GL_CCW ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(0.f, -1.f, 0.f)), glm::vec3(0.f), glm::vec3(0.f) });
    trisNum.push_back(segments);

    // INDICES
    for (unsigned int i = start; i < vertices.size() - 1; ++i) {

        unsigned int right = i + 2 == vertices.size() ? start : i + 1;

        indices.push_back(cullFace == GL_CCW ? i : right);
        indices.push_back(cullFace == GL_CCW ? right : i);
        indices.push_back(vertices.size() - 1);

        std::pair<glm::vec3, glm::vec3> TB;

        if (cullFace == GL_CCW) {
            TB = CalcTangentBitangent(vertices, i, right, vertices.size() - 1);
        }
        else {
            TB = CalcTangentBitangent(vertices, right, i, vertices.size() - 1);
        }

        vertices[i].Tangent += TB.first;
        vertices[i].Bitangent += TB.second;

        vertices[right].Tangent += TB.first;
        vertices[right].Bitangent += TB.second;

        vertices[vertices.size() - 1].Tangent += TB.first;
        vertices[vertices.size() - 1].Bitangent += TB.second;
    }

    for (unsigned int i = start; i < vertices.size(); ++i) {
        vertices[i].Tangent /= (float)trisNum[i - start];
        vertices[i].Tangent = glm::normalize(vertices[i].Tangent);

        vertices[i].Bitangent /= (float)trisNum[i - start];
        vertices[i].Bitangent = glm::normalize(vertices[i].Bitangent);
    }

    trisNum.clear();
}

bool ModelsManager::IsModelLoaded(const std::string& modelPath) {
    return _loadedModels.contains(_stringHash(modelPath));
}

bool ModelsManager::IsModelLoaded(size_t managerId) {
    return _loadedModels.contains(managerId);
}

InstantiatingModel ModelsManager::LoadModel(const std::string& modelPath)
{
    return InstantiatingModel(LoadModelData(modelPath));
}

InstantiatingModel ModelsManager::GetModel(size_t managerId) {
    InstantiatingModel model;
    if (_loadedModels.contains(managerId)) {
        model = _loadedModels[managerId];
    }
    return model;
}

InstantiatingModel ModelsManager::GetCube() {
    return GetModel(_stringHash(CUBE_PATH));
}

InstantiatingModel ModelsManager::GetPlane() {
    return GetModel(_stringHash(PLANE_PATH));
}

InstantiatingModel ModelsManager::GetSphere() {
    return GetModel(_stringHash(SPHERE_PATH));
}

InstantiatingModel ModelsManager::GetTorus() {
    return GetModel(_stringHash(TORUS_PATH));
}

InstantiatingModel ModelsManager::GetCone() {
    return GetModel(_stringHash(CONE_PATH));
}

InstantiatingModel ModelsManager::GetPiramid() {
    return GetModel(_stringHash(PIRAMID_PATH));
}

InstantiatingModel ModelsManager::GetTetrahedron() {
    return GetModel(_stringHash(TETRAHEDRON_PATH));
}

InstantiatingModel ModelsManager::GetCylinder() {
    return GetModel(_stringHash(CYLINDER_PATH));
}

InstantiatingModel ModelsManager::GetHexagon() {
    return GetModel(_stringHash(HEXAGON_PATH));
}

void ModelsManager::UnloadModel(const std::string& path) {
    UnloadModel(_stringHash(path));
}

void ModelsManager::UnloadModel(size_t managerId) {
    if (_loadedModels.contains(managerId))
    {
        ModelData* modelData = _loadedModels[managerId];
        for (InstantiatingMesh*& mesh : modelData->meshes)
        {
            delete mesh;
            mesh = nullptr;
        }

        modelData->meshes.clear();
        delete modelData;
        modelData = nullptr;
        _loadedModels.erase(managerId);
        _modelsPaths.erase(managerId);
    }
}

void ModelsManager::UnloadCube() {
    UnloadModel(CUBE_PATH);
}

void ModelsManager::UnloadPlane() {
    UnloadModel(PLANE_PATH);
}

void ModelsManager::UnloadSphere() {
    UnloadModel(SPHERE_PATH);
}

void ModelsManager::UnloadTorus() {
    UnloadModel(TORUS_PATH);
}

void ModelsManager::UnloadCone() {
    UnloadModel(CONE_PATH);
}

void ModelsManager::UnloadPiramid() {
    UnloadModel(PIRAMID_PATH);
}

void ModelsManager::UnloadTetrahedron() {
    UnloadModel(TETRAHEDRON_PATH);
}

void ModelsManager::UnloadCylinder() {
    UnloadModel(CYLINDER_PATH);
}

void ModelsManager::UnloadHexagon() {
    UnloadModel(HEXAGON_PATH);
}

void ModelsManager::UnloadAll() {
    
    for (auto item : _loadedModels) {
        for (InstantiatingMesh* mesh : item.second->meshes)
        {
            delete mesh;
        }

        item.second->meshes.clear();
        delete item.second;
    }

    _loadedModels.clear();
    _modelsPaths.clear();
}

InstantiatingModel ModelsManager::CreateModel(const std::string& modelName, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    size_t strHash = _stringHash(modelName);

    if (!_loadedModels.contains(strHash))
    {
        SPDLOG_INFO("Creating model: {}!", modelName);

        _loadedModels.emplace(strHash, new ModelData {
            .id = strHash,
            .meshes { new InstantiatingMesh(vertices, indices) }
            }
        );

        vertices.clear();
        indices.clear();
    }
    else
    {
        SPDLOG_INFO("Model already exist: {}!", modelName);
    }

    return _loadedModels[strHash];
}

std::string ModelsManager::GetModelName(size_t id)
{
    if (_loadedModels.count(id) == 0) {
        SPDLOG_ERROR("ModelsManager::Model not found");
        return "";
    }

    std::string p = _modelsPaths[id];
    return std::filesystem::path(p).stem().string();
}

std::string ModelsManager::GetModelName(const std::string& modelPath)
{
    return GetModelName(_stringHash(modelPath));
}

std::map<size_t, std::string> ModelsManager::GetAllModelsNames()
{
    std::map<size_t, std::string> names = std::map<size_t, std::string>();

    for (auto item : _modelsPaths) {
        if (std::regex_match(item.second, std::regex("[{]\\w+[_](?:GENERATED)[_]\\d+[}]"))) continue;
        names[item.first] = std::filesystem::path(item.second).stem().string();
    }
    return names;
}

YAML::Node ModelsManager::Serialize()
{
    YAML::Node models;
    size_t id = 0;
    for (const auto& modelPair : _modelsPaths) {

        if (std::regex_match(modelPair.second, std::regex("[{]\\w+[_](?:GENERATED)[_]\\d+[}]"))) continue;

        YAML::Node model;
        model["id"] = id++;
        model["path"] = modelPair.second;
        models.push_back(model);
    }
    return models;
}

#if _DEBUG
void ModelsManager::DrawEditor(bool* p_open)
{
    if (!ImGui::Begin("Models Manager", p_open, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Load##Models Manager Load Menu"))
        {
            if (ImGui::MenuItem("Load From File##Models Manager Menu Load", NULL, &_fileDialogOpen)) {
                _fileDialogOpen = true;
                _fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
                _fileDialogInfo.title = "Open File##Models Manager";
                _fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\models");
            }

            std::vector<std::string> avilPredModels = {
                CUBE_PATH,
                PLANE_PATH,
                SPHERE_PATH,
                TORUS_PATH,
                CONE_PATH,
                PIRAMID_PATH,
                TETRAHEDRON_PATH,
                CYLINDER_PATH,
                HEXAGON_PATH
            };

            for (int i = avilPredModels.size() - 1; i > -1; --i)
                if (IsModelLoaded(avilPredModels[i]))
                    avilPredModels.erase(avilPredModels.begin() + i);

            if (ImGui::BeginMenu("Load Defined Object##Models Manager Menu Load", avilPredModels.size() != 0)) {
                
                for (auto& item : avilPredModels) {
                    if (ImGui::MenuItem(item.c_str())) {
                        LoadModel(item);
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
    {
        // Result path in: m_fileDialogInfo.resultPath
        LoadModel(std::filesystem::relative(_fileDialogInfo.resultPath).string());
    }

    ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool node_open = ImGui::TreeNodeEx(std::string("Models##Model Manager").c_str(), node_flag);

    std::list<size_t> clicked = std::list<size_t>();
    clicked.clear();
    if (node_open) {
        size_t i = 0;
        for (auto& item : _modelsPaths) {

            if (std::regex_match(item.second, std::regex("[{]\\w+[_](?:GENERATED)[_]\\d+[}]"))) continue;

            std::string n = GetModelName(item.second);
            ImGui::BulletText(n.c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
            if (ImGui::RemoveButton(std::string("##Remove Models Manager").append(std::to_string(i)).c_str())) {
                clicked.push_back(item.first);
            }
            ++i;
        }
        ImGui::TreePop();
    }

    if (clicked.size() > 0) {
        clicked.sort();

        for (int i = clicked.size() - 1; i > -1; --i)
        {
            UnloadModel(clicked.back());

            clicked.pop_back();
        }
    }

    clicked.clear();

    ImGui::End();
}
#endif