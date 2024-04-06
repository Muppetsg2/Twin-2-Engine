#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

std::hash<std::string> ModelsManager::stringHash;
std::list<ModelData*> ModelsManager::loadedModels;

ModelData* ModelsManager::LoadModel(const std::string& modelPath)
{
    size_t strHash = stringHash(modelPath);

    std::list<ModelData*>::iterator found =
        std::find_if(loadedModels.begin(), loadedModels.end(), [strHash](ModelData* data) { return data->modelHash == strHash; });
    
    ModelData* modelData;
    if (found == loadedModels.end())
    {
        SPDLOG_INFO("Loading model: {}!", modelPath);

        Model* model = new Model(modelPath.c_str());

        //Creating SSBO

        GLuint ssbo;
        glGenBuffers(1, &ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        modelData = new ModelData{
            .modelHash = strHash,
            .useNumber = 1,
            .model = model,
            .ssboId = ssbo
        };

        for (Mesh& mesh : model->meshes)
        {
            modelData->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        }

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
    //std::cout << "Tutaj6\n";
    if (modelData != nullptr)
    {
        //std::cout << "Tutaj7\n";
        modelData->useNumber--;
        if (modelData->useNumber == 0)
        {
            if (modelData == _cubeModel || modelData == _piramidModel || modelData == _sphereModel || modelData == _planeModel) {
                if (modelData == _cubeModel) _cubeModel = nullptr;
                else if (modelData == _piramidModel) _piramidModel = nullptr;
                else if (modelData == _sphereModel) _sphereModel = nullptr;
                else if (modelData == _planeModel) _planeModel = nullptr;
                delete modelData->model;
                glDeleteBuffers(1, &modelData->ssboId);
                for (auto& m : modelData->meshes) {
                    glDeleteBuffers(1, &m.mesh->EBO);
                    glDeleteBuffers(1, &m.mesh->VBO);
                    glDeleteVertexArrays(1, &m.mesh->VAO);
                }
                delete modelData;
            }
            else if (loadedModels.size() != 0) {
                //std::cout << "Tutaj8\n";
                std::list<ModelData*>::iterator found = std::find_if(loadedModels.begin(), loadedModels.end(), [modelData](Twin2Engine::GraphicEngine::ModelData* data) { return data == modelData; });

                if (found != loadedModels.end())
                {
                    delete (*found)->model;
                    glDeleteBuffers(1, &(*found)->ssboId);
                    for (auto& m : (*found)->meshes) {
                        glDeleteBuffers(1, &m.mesh->EBO);
                        glDeleteBuffers(1, &m.mesh->VBO);
                        glDeleteVertexArrays(1, &m.mesh->VAO);
                    }
                    loadedModels.erase(found);
                    delete (*found);
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

        vector<Vertex> vertices {
            // Fron Face
            {.Position = glm::vec3(-0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 0.0f) },

            // Back Face
            {.Position = glm::vec3( 0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3(-0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(1.0f, 0.0f) },

            // Left Face
            {.Position = glm::vec3(-0.5f,  0.5f, -0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3(-0.5f,  0.5f,  0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f) },

            // Right Face
            {.Position = glm::vec3( 0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f) },

            // Top Face
            {.Position = glm::vec3(-0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3(-0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f) },

            // Bottom Face
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f) }
        };

        vector<unsigned int> indices {
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

        Model* model = new Model(vertices, indices, vector<Texture>());

        //Creating SSBO
        GLuint ssbo;
        glGenBuffers(1, &ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        _cubeModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1,
            .model = model,
            .ssboId = ssbo
        };

        for (Mesh& mesh : model->meshes)
        {
            _cubeModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        }
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

        Model* model = new Model(vertices, indices, vector<Texture>());

        //Creating SSBO
        GLuint ssbo;
        glGenBuffers(1, &ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        _sphereModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1,
            .model = model,
            .ssboId = ssbo
        };

        for (Mesh& mesh : model->meshes)
        {
            _sphereModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        }
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

        vector<Vertex> vertices {
            {.Position = glm::vec3(-0.5f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
        };

        vector<unsigned int> indices {
            0, 1, 2,
            2, 1, 3
        };

        Model* model = new Model(vertices, indices, vector<Texture>());

        //Creating SSBO
        GLuint ssbo;
        glGenBuffers(1, &ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        _planeModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1,
            .model = model,
            .ssboId = ssbo
        };

        for (Mesh& mesh : model->meshes)
        {
            _planeModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        }
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

        vector<Vertex> vertices{
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3(-0.5f,  0.0f,  0.0f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f, -1.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 0.0f) },

            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(0.5f, 0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 0.0f,  0.0f, -1.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },

            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(0.5f, 0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 0.0f,  0.0f,  1.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },

            {.Position = glm::vec3(-0.5f, -0.5f, -0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f) },
            {.Position = glm::vec3(-0.5f, -0.5f,  0.5f), .Normal = glm::vec3(-1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) },

            {.Position = glm::vec3( 0.5f, -0.5f,  0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) },
            {.Position = glm::vec3( 0.0f,  0.5f,  0.0f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(0.5f, 0.0f) },
            {.Position = glm::vec3( 0.5f, -0.5f, -0.5f), .Normal = glm::vec3( 1.0f,  0.0f,  0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) }
        };

        vector<unsigned int> indices{
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

        Model* model = new Model(vertices, indices, vector<Texture>());

        //Creating SSBO
        GLuint ssbo;
        glGenBuffers(1, &ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        _piramidModel = new ModelData{
            .modelHash = 0,
            .useNumber = 1,
            .model = model,
            .ssboId = ssbo
        };

        for (Mesh& mesh : model->meshes)
        {
            _piramidModel->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        }
    }
    else
    {
        SPDLOG_INFO("Piramid Model Already Created");
        _piramidModel->useNumber++;
    }
    return _piramidModel;
}

InstatiatingModel ModelsManager::CreateModel(const std::string& modelName, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    size_t strHash = stringHash(modelName);

    std::list<ModelData*>::iterator found =
        std::find_if(loadedModels.begin(), loadedModels.end(), [strHash](ModelData* data) { return data->modelHash == strHash; });

    ModelData* modelData;
    if (found == loadedModels.end())
    {
        SPDLOG_INFO("Creating model: {}!", modelName);

        Model* model = new Model(vertices, indices, textures);

        //Creating SSBO

        GLuint ssbo;
        glGenBuffers(1, &ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        modelData = new ModelData{
            .modelHash = strHash,
            .useNumber = 1,
            .model = model,
            .ssboId = ssbo
        };

        for (Mesh& mesh : model->meshes)
        {
            modelData->meshes.push_back(InstatiatingMesh(&mesh, ssbo));
        }

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

//void GraphicEngine::ModelsManager::FreeModel(InstatiatingModel*& model)
//{
//    model->modelData->useNumber--;
//    model = nullptr;
//}