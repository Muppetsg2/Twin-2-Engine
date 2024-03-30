#include <ModelsManager.h>
#include <InstatiatingMesh.h>

//using ModelData;

using namespace Twin2Engine::GraphicEngine;

std::hash<std::string> ModelsManager::stringHash;
std::list<ModelData*> ModelsManager::loadedModels;


Twin2Engine::GraphicEngine::ModelData* Twin2Engine::GraphicEngine::ModelsManager::LoadModel(const std::string& modelPath)
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

                delete (*found)->model;
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

InstatiatingModel Twin2Engine::GraphicEngine::ModelsManager::CreateModel(const std::string& modelName, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
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