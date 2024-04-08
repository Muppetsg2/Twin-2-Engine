#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

std::hash<std::string> ModelsManager::stringHash;
std::list<ModelData*> ModelsManager::loadedModels;


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

        // Normal
        if (mesh->HasNormals()) {
            vertices[i].Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        // Texture coordinates
        if (mesh->HasTextureCoords(0)) {
            vertices[i].TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
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

ModelData* ModelsManager::LoadModel(const std::string& modelPath)
{
    size_t strHash = stringHash(modelPath);

    std::list<ModelData*>::iterator found =
        std::find_if(loadedModels.begin(), loadedModels.end(), [strHash](ModelData* data) { return data->modelHash == strHash; });

    ModelData* modelData;
    if (found == loadedModels.end())
    {
        SPDLOG_INFO("Loading model: {}!", modelPath);

        modelData = new ModelData{
            .modelHash = strHash,
            .useNumber = 1
        };

#if ASSIMP_LOADING
        LoadModelAssimp(modelPath, modelData);
#elif TINYGLTF_LOADING
        LoadModelGLTF(modelPath, modelData);
#endif

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

void ModelsManager::UnloadModel(ModelData* modelData)
{
    if (modelData != nullptr)
    {
        modelData->useNumber--;
        if (modelData->useNumber == 0)
        {
            if (modelData == _cubeModel || modelData == _piramidModel || modelData == _sphereModel || modelData == _planeModel) {
                if (modelData == _cubeModel) _cubeModel = nullptr;
                else if (modelData == _piramidModel) _piramidModel = nullptr;
                else if (modelData == _sphereModel) _sphereModel = nullptr;
                else if (modelData == _planeModel) _planeModel = nullptr;
                for (auto& mesh : modelData->meshes) {
                    delete mesh;
                }
                delete modelData;
            }
            else if (loadedModels.size() != 0) {
                std::list<ModelData*>::iterator found = std::find_if(loadedModels.begin(), loadedModels.end(), [modelData](Twin2Engine::GraphicEngine::ModelData* data) { return data == modelData; });

                if (found != loadedModels.end())
                {
                    for (InstatiatingMesh*& mesh : (*found)->meshes) 
                    {
                        delete mesh;
                    }

                    delete (*found);
                    loadedModels.erase(found);
                }
            }
            else
            {
                SPDLOG_ERROR("THIS INSTRUCTION SHOULDNT HAPPEND");
            }
        }
    }
}

void ModelsManager::Init()
{

}

void ModelsManager::End()
{

}

InstatiatingModel ModelsManager::GetModel(const std::string& modelPath)
{
    ModelData* modelData = LoadModel(modelPath);

    InstatiatingModel model = InstatiatingModel(modelData);

    return model;
}

ModelData* ModelsManager::_cubeModel = nullptr;
InstatiatingModel ModelsManager::GetCube()
{
    if (_cubeModel == nullptr)
    {
        SPDLOG_INFO("Creating Simple Cube Model");

        std::vector<Vertex> vertices {
            // Fron Face
            {.Position = glm::vec3(-0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            // Back Face
            {.Position = glm::vec3( 0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            // Left Face
            {.Position = glm::vec3(-0.5f,  0.5f, -0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f,  0.5f,  0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            // Right Face
            {.Position = glm::vec3( 0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            // Top Face
            {.Position = glm::vec3(-0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            // Bottom Face
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) }
        };

        std::vector<unsigned int> indices {
            // Fron Face
            0, 1, 2,
            2, 1, 3,

            // Back Face
            4, 5, 6,
            6, 5, 7,

            // Left Face
            8, 9, 10,
            10, 9, 11,

            // Right Face
            12, 13, 14,
            14, 13, 15,

            // Top Face
            16, 17, 18,
            18, 17, 19,

            // Bottom Face
            22, 21, 20,
            23, 21, 22
        };

        InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);

        _cubeModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1
        };
        _cubeModel->meshes.push_back(mesh);

        //Creating SSBO
        //GLuint ssbo;
        //glGenBuffers(1, &ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        //_cubeModel = new ModelData{
        //    .modelHash = 0,
        //    .useNumber = 1
        //    .model = model,
        //    .ssboId = ssbo
        //};
        // 
        //for (Mesh& mesh : model->meshes)
        //{
        //    _cubeModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        //}
    }
    else
    {
        SPDLOG_INFO("Cube Model Already Created");
        _cubeModel->useNumber++;
    }
    return _cubeModel;
}

ModelData* ModelsManager::_sphereModel = nullptr;
InstatiatingModel ModelsManager::GetSphere()
{
    if (_sphereModel == nullptr)
    {
        SPDLOG_INFO("Creating Simple Sphere Model");

        uint32_t horizontalSegments = 12;
        uint32_t verticalSegments = 12;

        float angleYDiff = 180.f / horizontalSegments;
        float angleXZDiff = 360.f / verticalSegments;

        std::vector<glm::vec3> verts = std::vector<glm::vec3>();
        std::vector<glm::vec2> texCoords = std::vector<glm::vec2>();
        std::vector<glm::vec3> normals = std::vector<glm::vec3>();

        // VERTICIES
        // TOP VERTEX
        verts.push_back({ 0.f, 1.f, 0.f });
        // TOP HALF AND BOTTOM HALF
        float angleY = angleYDiff;
        for (unsigned int i = 0; i < horizontalSegments - 1; ++i) {
            float radiansY = glm::radians(angleY);
            float r = sinf(radiansY);
            float y = cosf(radiansY);
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
                }
                angleXZ += angleXZDiff;
            }
            angleY += angleYDiff;
        }
        // BOTTOM VERTEX
        verts.push_back({ 0.f, -1.f, 0.f });

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

        // INDICIES
        std::vector<unsigned int> indices = std::vector<unsigned int>();
        // TOP CIRCLE + BOTTOM CIRCLE
        for (unsigned int i = 0; i < verticalSegments; ++i) {
            // TOP CIRCLE
            indices.push_back(i + 1);
            indices.push_back(0);
            indices.push_back((i + 1) + 1);

            // BOTTOM CIRCLE
            indices.push_back(verticiesNum - 1);
            indices.push_back(verticiesNum - 2 - verticalSegments - 1 + i + 1);
            indices.push_back(verticiesNum - 2 - verticalSegments - 1 + (i + 1) + 1);
        }
        // CENTER CIRCLES
        for (unsigned int c = 0; c < horizontalSegments - 2; ++c) {
            unsigned int startV = c * (verticalSegments + 1) + 1;
            for (unsigned int i = 0; i < verticalSegments; ++i) {
                unsigned int topLeft = i + startV;
                unsigned int topRight = (i + 1) + startV;
                unsigned int bottomLeft = i + verticalSegments + 1 + startV;
                unsigned int bottomRight = (i + 1) + verticalSegments + 1 + startV;

                indices.push_back(bottomLeft);
                indices.push_back(topLeft);
                indices.push_back(topRight);

                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                indices.push_back(bottomRight);
            }
        }

        // FULL VERTICES
        std::vector<Vertex> vertices = std::vector<Vertex>();
        for (size_t i = 0; i < verticiesNum; ++i)
        {
            vertices.push_back({ .Position = verts[i], .Normal = normals[i], .TexCoords = texCoords[i] });
        }

        verts.clear();
        texCoords.clear();
        normals.clear();

        InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);

        _sphereModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1
        };

        _sphereModel->meshes.push_back(mesh);

        //Model* model = new Model(vertices, indices, vector<Texture>());
        //
        ////Creating SSBO
        //GLuint ssbo;
        //glGenBuffers(1, &ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //
        //_sphereModel = new ModelData{
        //    .modelHash = 0,
        //    .useNumber = 1,
        //    .model = model,
        //    .ssboId = ssbo
        //};
        //
        //for (Mesh& mesh : model->meshes)
        //{
        //    _sphereModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        //}
    }
    else
    {
        SPDLOG_INFO("Sphere Model Already Created");
        _sphereModel->useNumber++;
    }
    return _sphereModel;
}

ModelData* ModelsManager::_planeModel = nullptr;
InstatiatingModel ModelsManager::GetPlane()
{
    if (_planeModel == nullptr)
    {
        SPDLOG_INFO("Creating Simple Plane Model");

        std::vector<Vertex> vertices {
            { .Position = glm::vec3(-0.5f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            { .Position = glm::vec3(-0.5f, -0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            { .Position = glm::vec3( 0.5f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            { .Position = glm::vec3( 0.5f, -0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
        };

        std::vector<unsigned int> indices {
            0, 1, 2,
            2, 1, 3
        };

        InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);

        _planeModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1
        };

        _planeModel->meshes.push_back(mesh);

        //Model* model = new Model(vertices, indices, vector<Texture>());
        //
        ////Creating SSBO
        //GLuint ssbo;
        //glGenBuffers(1, &ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //
        //_planeModel = new ModelData{
        //    .modelHash = 0,
        //    .useNumber = 1,
        //    .model = model,
        //    .ssboId = ssbo
        //};
        //
        //for (Mesh& mesh : model->meshes)
        //{
        //    _planeModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        //}
    }
    else
    {
        SPDLOG_INFO("Plane Model Already Created");
        _planeModel->useNumber++;
    }
    return _planeModel;
}

ModelData* ModelsManager::_piramidModel = nullptr;
InstatiatingModel ModelsManager::GetPiramid()
{
    if (_piramidModel == nullptr)
    {
        SPDLOG_INFO("Creating Simple Piramid Model");

        std::vector<Vertex> vertices{
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f,  0.0f,  0.0f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },

            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f), .Tangent = glm::vec3(0.0f), .Bitangent = glm::vec3(0.0f) }
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

        InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);

        _piramidModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1
        };

        _piramidModel->meshes.push_back(mesh);

        //Model* model = new Model(vertices, indices, vector<Texture>());
        //
        ////Creating SSBO
        //GLuint ssbo;
        //glGenBuffers(1, &ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //
        //_piramidModel = new ModelData{
        //    .modelHash = 0,
        //    .useNumber = 1,
        //    .model = model,
        //    .ssboId = ssbo
        //};
        //
        //for (Mesh& mesh : model->meshes)
        //{
        //    _piramidModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        //}
    }
    else
    {
        SPDLOG_INFO("Piramid Model Already Created");
        _piramidModel->useNumber++;
    }
    return _piramidModel;
}

InstatiatingModel ModelsManager::CreateModel(const std::string& modelName, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    size_t strHash = stringHash(modelName);

    std::list<ModelData*>::iterator found =
        std::find_if(loadedModels.begin(), loadedModels.end(), [strHash](ModelData* data) { return data->modelHash == strHash; });

    ModelData* modelData;
    if (found == loadedModels.end())
    {
        SPDLOG_INFO("Creating model: {}!", modelName);

        InstatiatingMesh* mesh = new InstatiatingMesh(vertices, indices);

        ModelData* newModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1
        };

        newModel->meshes.push_back(mesh);
        //Model* model = new Model(vertices, indices, textures);
        //
        ////Creating SSBO
        //
        //GLuint ssbo;
        //glGenBuffers(1, &ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //
        //modelData = new ModelData{
        //    .modelHash = strHash,
        //    .useNumber = 1,
        //    .model = model,
        //    .ssboId = ssbo
        //};
        //
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

    return modelData;
}
