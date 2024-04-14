#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

std::hash<std::string> ModelsManager::stringHash;
std::map<size_t, ModelData*> ModelsManager::loadedModels;


#if ASSIMP_LOADING

inline void ModelsManager::LoadModelAssimp(const std::string& modelPath, ModelData* modelData)
{
    // Create an instance of the Assimp importer
    Assimp::Importer importer;

    // Read the file with Assimp
    const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

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

inline void ModelsManager::ExtractMeshAssimp(const aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    // Reserve memory for vertices
    vertices.resize(mesh->mNumVertices);

    // Extract vertex data
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        // Position
        vertices[i].Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Texture coordinates
        if (mesh->HasTextureCoords(0)) {
            vertices[i].TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }

        // Normal
        if (mesh->HasNormals()) {
            vertices[i].Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        if (mesh->HasTangentsAndBitangents()) {
            vertices[i].Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertices[i].Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
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

    modelData->meshes.resize(1);
    modelData->meshes[0] = new InstatiatingMesh(vertices, indices);
}

void ModelsManager::LoadSphere(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Sphere Model");

    uint32_t horizontalSegments = 12;
    uint32_t verticalSegments = 12;

    float angleYDiff = 180.f / horizontalSegments;
    float angleXZDiff = 360.f / verticalSegments;

    std::vector<glm::vec3> verts = std::vector<glm::vec3>();
    std::vector<glm::vec2> texCoords = std::vector<glm::vec2>();
    std::vector<glm::vec3> normals = std::vector<glm::vec3>();
    std::vector<unsigned int> numTrinagles = std::vector<unsigned int>();

    // VERTICIES AND NUMBER OF TRIANGLES
    // TOP VERTEX
    verts.push_back({ 0.f, 1.f, 0.f });
    numTrinagles.push_back(verticalSegments);
    // TOP HALF AND BOTTOM HALF
    float angleY = angleYDiff;
    for (unsigned int i = 0; i < horizontalSegments - 1; ++i) {
        float radiansY = glm::radians(angleY);
        float r = sinf(radiansY);
        float y = cosf(radiansY);

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
            verts.push_back({ x, y, z });
            if (j == verticalSegments - 1)
            {
                // Add first in the end again for texCoords
                verts.push_back({ r * sinf(glm::radians(0.f)), y, r * cosf(glm::radians(0.f)) });
                numTrinagles.push_back(t);
            }
            numTrinagles.push_back(t);
            angleXZ += angleXZDiff;
        }
        angleY += angleYDiff;
    }
    // BOTTOM VERTEX
    verts.push_back({ 0.f, -1.f, 0.f });
    numTrinagles.push_back(verticalSegments);

    // TEXTURE COORDS
    // TOP VERTEX
    texCoords.push_back({ .5f, 1.f });

    float horizontalDiff = 1.f / (float)horizontalSegments;
    float verticalDiff = 1.f / (float)verticalSegments;

    // CENTER CIRCLES
    size_t verticiesNum = verts.size();
    for (unsigned int c = 0; c < horizontalSegments - 1; ++c) {
        unsigned int startV = c * verticalSegments + 1;
        for (unsigned int i = 0; i < verticalSegments; ++i) {

            texCoords.push_back({ i * verticalDiff, 1.f - horizontalDiff * (c + 1) });

            if ((i + 1) % verticalSegments == 0)
            {
                texCoords.push_back({ 1.f , 1.f - horizontalDiff * (c + 1) });
            }
        }
    }

    // BOTTOM VERTEX
    texCoords.push_back({ .5f, 0.f });

    // NORMALS
    for (size_t i = 0; i < verts.size(); ++i)
    {
        if (glm::length(verts[i]) != 0.f) {
            normals.push_back(glm::normalize(verts[i]));
        }
        else {
            normals.push_back(verts[i]);
        }
    }

    // VERTICES WITHOUT TANGENTS AND BITANGENTS
    std::vector<Vertex> vertices = std::vector<Vertex>();
    for (size_t i = 0; i < verticiesNum; ++i)
    {
        vertices.push_back({ verts[i], texCoords[i], normals[i], glm::vec3(0.f), glm::vec3(0.f) });
    }

    // INDICIES, TANGENTS AND BITANGENTS
    std::pair<glm::vec3, glm::vec3> TB;

    std::vector<unsigned int> indices = std::vector<unsigned int>();
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
        rightVertex = verticiesNum - 2 - verticalSegments - 1 + (i + 1) + 1;
        leftVertex = verticiesNum - 2 - verticalSegments - 1 + i + 1;
        topVertex = verticiesNum - 1;

        indices.push_back(rightVertex);
        indices.push_back(leftVertex);
        indices.push_back(topVertex);

        TB = CalcTangentBitangent(vertices, rightVertex, topVertex, leftVertex);

        vertices[rightVertex].Tangent += TB.first;
        vertices[rightVertex].Bitangent += TB.second;

        vertices[topVertex].Tangent += TB.first;
        vertices[topVertex].Bitangent += TB.second;

        vertices[leftVertex].Tangent += TB.first;
        vertices[leftVertex].Bitangent += TB.second;
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

    // FULL VERTICES
    for (unsigned int i = 0; i < vertices.size(); ++i) {
        vertices[i].Tangent /= (float)numTrinagles[i];
        vertices[i].Tangent = glm::normalize(vertices[i].Tangent);

        vertices[i].Bitangent /= (float)numTrinagles[i];
        vertices[i].Bitangent = glm::normalize(vertices[i].Bitangent);
    }

    verts.clear();
    texCoords.clear();
    normals.clear();

    modelData->meshes.resize(1);
    modelData->meshes[0] = new InstatiatingMesh(vertices, indices);
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

    modelData->meshes.resize(1);
    modelData->meshes[0] = new InstatiatingMesh(vertices, indices);
}

void ModelsManager::LoadPiramid(ModelData* modelData)
{
    SPDLOG_INFO("Creating Simple Piramid Model");

    std::vector<Vertex> vertices{
        {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f, -1.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(-0.5f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, -0.5f), .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f, -1.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f,  0.5f), .TexCoords = glm::vec2(1.0f, 0.0f), .Normal = glm::vec3(0.0f, -1.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

        {.Position = glm::vec3(0.5f, -0.5f, -0.5f), .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f,  0.0f, -1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.0f,  0.5f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(0.0f,  0.0f, -1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f,  0.0f, -1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

        {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(0.0f,  0.0f,  1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.0f,  0.5f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(0.0f,  0.0f,  1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f,  0.5f), .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(0.0f,  0.0f,  1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

        {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.0f,  0.5f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

        {.Position = glm::vec3(0.5f, -0.5f,  0.5f), .TexCoords = glm::vec2(0.0f, 1.0f), .Normal = glm::vec3(1.0f,  0.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.0f,  0.5f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Normal = glm::vec3(1.0f,  0.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        {.Position = glm::vec3(0.5f, -0.5f, -0.5f), .TexCoords = glm::vec2(1.0f, 1.0f), .Normal = glm::vec3(1.0f,  0.0f,  0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) }
    };

    std::vector<unsigned int> indices{
        // Bottom Face
        0, 1, 2,
        2, 1, 3,

        // Front Triangle
        4, 5, 6,

        // Back Triangle
        7, 8, 9,

        // Left Triangle
        10, 11, 12,

        // Right Triangle
        13, 14, 15
    };

    modelData->meshes.resize(1);
    modelData->meshes[0] = new InstatiatingMesh(vertices, indices);
}

ModelData* ModelsManager::LoadModelData(const std::string& modelPath)
{
    size_t strHash = stringHash(modelPath);

    ModelData* modelData;
    if (loadedModels.find(strHash) == loadedModels.end())
    {
        SPDLOG_INFO("Loading model: {}!", modelPath);

        modelData = new ModelData{
            .id = strHash,
        };

        if (modelPath == CUBE_PATH) {
            LoadCube(modelData);
        }
        else if (modelPath == SPHERE_PATH) {
            LoadSphere(modelData);
        }
        else if (modelPath == PIRAMID_PATH) {
            LoadPiramid(modelData);
        }
        else if (modelPath == PLANE_PATH) {
            LoadPlane(modelData);
        }
        else {
#if ASSIMP_LOADING
            LoadModelAssimp(modelPath, modelData);
#elif TINYGLTF_LOADING
            LoadModelGLTF(modelPath, modelData);
#endif
        }

        loadedModels[strHash] = modelData;
    }
    else
    {
        SPDLOG_INFO("Model already loaded: {}!", modelPath);
        modelData = loadedModels[strHash];
    }

    return modelData;
}

void ModelsManager::UnloadModel(const std::string& path) {
    UnloadModel(stringHash(path));
}

void ModelsManager::UnloadModel(size_t managerId) {
    if (loadedModels.find(managerId) != loadedModels.end())
    {
        ModelData* modelData = loadedModels[managerId];
        for (InstatiatingMesh*& mesh : modelData->meshes)
        {
            delete mesh;
        }

        delete modelData;
        loadedModels.erase(managerId);
    }
}

void ModelsManager::UnloadCube() {
    UnloadModel(CUBE_PATH);
}

void ModelsManager::UnloadSphere() {
    UnloadModel(SPHERE_PATH);
}

void ModelsManager::UnloadPlane() {
    UnloadModel(PLANE_PATH);
}

void ModelsManager::UnloadPiramid() {
    UnloadModel(PIRAMID_PATH);
}

std::pair<glm::vec3, glm::vec3> ModelsManager::CalcTangentBitangent(std::vector<Vertex> vertices, unsigned int i1, unsigned int i2, unsigned int i3)
{
    std::pair<glm::vec3, glm::vec3> TB;

    Vertex v0 = vertices[i1];
    Vertex v1 = vertices[i2];
    Vertex v2 = vertices[i3];

    glm::vec3 pos0 = v0.Position;
    glm::vec3 pos1 = v1.Position;
    glm::vec3 pos2 = v2.Position;

    glm::vec2 uv0 = v0.TexCoords;
    glm::vec2 uv1 = v1.TexCoords;
    glm::vec2 uv2 = v2.TexCoords;

    glm::vec3 delta_pos1 = pos1 - pos0;
    glm::vec3 delta_pos2 = pos2 - pos0;

    glm::vec2 delta_uv1 = uv1 - uv0;
    glm::vec2 delta_uv2 = uv2 - uv0;

    float r = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);

    // Save the results
    TB.first = (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * r;
    TB.second = (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) * r;

    return TB;
}

InstatiatingModel ModelsManager::LoadModel(const std::string& modelPath)
{
    ModelData* modelData = LoadModelData(modelPath);

    InstatiatingModel model = InstatiatingModel(modelData);

    return model;
}

InstatiatingModel ModelsManager::GetModel(size_t managerId) {
    InstatiatingModel model;
    if (loadedModels.find(managerId) != loadedModels.end()) {
        model = loadedModels[managerId];
    }
    return model;
}

InstatiatingModel ModelsManager::GetCube() {
    return GetModel(stringHash(CUBE_PATH));
}

InstatiatingModel ModelsManager::GetSphere() {
    return GetModel(stringHash(SPHERE_PATH));
}

InstatiatingModel ModelsManager::GetPlane() {
    return GetModel(stringHash(PLANE_PATH));
}

InstatiatingModel ModelsManager::GetPiramid() {
    return GetModel(stringHash(PIRAMID_PATH));
}

InstatiatingModel ModelsManager::CreateModel(const std::string& modelName, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    size_t strHash = stringHash(modelName);

    ModelData* modelData = nullptr;
    if (loadedModels.find(strHash) == loadedModels.end())
    {
        SPDLOG_INFO("Creating model: {}!", modelName);

        InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);

        modelData = new ModelData{
            .id = strHash,
        };

        modelData->meshes.push_back(mesh);
        loadedModels[strHash] = modelData;
    }
    else
    {
        SPDLOG_INFO("Model already exist: {}!", modelName);
        modelData = loadedModels[strHash];
    }

    return modelData;
}