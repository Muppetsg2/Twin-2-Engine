#include <InstatiatingModel.h>

#include <ModelsManager.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::GraphicEngine;

void Twin2Engine::GraphicEngine::InstatiatingModel::setupModel(const vector<glm::mat4>& transforms)
{
    //for (Mesh& mesh : model->meshes)
    {
        //meshes.push_back(InstatiatingMesh(&mesh, transforms));
    }
}


//InstatiatingModel::InstatiatingModel(const char* path, const vector<glm::mat4>& transforms)
//{
//    model = new Model(path);
//
//    setupModel(transforms);
//}
Twin2Engine::GraphicEngine::InstatiatingModel::InstatiatingModel(ModelData* modelData)
{
    this->modelData = modelData;
}

Twin2Engine::GraphicEngine::InstatiatingModel::InstatiatingModel(const InstatiatingModel& model)
{
    modelData = model.modelData;

    modelData->useNumber++;
}

Twin2Engine::GraphicEngine::InstatiatingModel::InstatiatingModel(InstatiatingModel&& model)
{
    modelData = model.modelData;
    modelData->useNumber++;

}

Twin2Engine::GraphicEngine::InstatiatingModel::InstatiatingModel()
{
    modelData = nullptr;
}

Twin2Engine::GraphicEngine::InstatiatingModel::~InstatiatingModel()
{
    Twin2Engine::GraphicEngine::ModelsManager::UnloadModel(modelData);
}

size_t Twin2Engine::GraphicEngine::InstatiatingModel::GetMeshCount() const
{
    if (modelData == nullptr)
    {
        return 0;
    }

    return modelData->meshes.size();
}

Twin2Engine::GraphicEngine::InstatiatingMesh* Twin2Engine::GraphicEngine::InstatiatingModel::GetMesh(size_t index) const
{
    if (modelData == nullptr)
    {
        return nullptr;
    }

    if (index >= modelData->meshes.size())
    {
        index = modelData->meshes.size() - 1;
    }

    return &(modelData->meshes[index]);
}

void Twin2Engine::GraphicEngine::InstatiatingModel::Draw(Shader* shaderProgram)
{

    SPDLOG_ERROR("Not implemented!");
    //for (unsigned int i = 0; i < meshes.size(); i++)
    //    meshes[i].Draw(shaderProgram);
}

void Twin2Engine::GraphicEngine::InstatiatingModel::SetTransform(unsigned int meshIndex, const vector<Transform*> transforms)
{
    vector<glm::mat4> transformsMatrixes;
    for (Transform* transform : transforms)
    {
        //transform->RecalculateTransform();
        //if (transform->GerHierachyActive())
        //{
        //    transformsMatrixes.push_back(transform->transform);
        //}
    }

    //meshes[meshIndex].SetNewTransforms(transformsMatrixes);
}

Twin2Engine::GraphicEngine::InstatiatingModel& Twin2Engine::GraphicEngine::InstatiatingModel::operator=(const InstatiatingModel& other)
{
    Twin2Engine::GraphicEngine::ModelsManager::UnloadModel(modelData);
    modelData = other.modelData;
    modelData->useNumber++;
    return *this;
}

Twin2Engine::GraphicEngine::InstatiatingModel& Twin2Engine::GraphicEngine::InstatiatingModel::operator=(InstatiatingModel&& other)
{
    Twin2Engine::GraphicEngine::ModelsManager::UnloadModel(modelData);
    modelData = other.modelData;
    modelData->useNumber++;

    return *this;
}

Twin2Engine::GraphicEngine::InstatiatingModel& Twin2Engine::GraphicEngine::InstatiatingModel::operator=(std::nullptr_t)
{
    Twin2Engine::GraphicEngine::ModelsManager::UnloadModel(modelData);

    return *this;
}

bool Twin2Engine::GraphicEngine::InstatiatingModel::operator==(std::nullptr_t)
{
    return modelData == nullptr;
}

bool Twin2Engine::GraphicEngine::InstatiatingModel::operator!=(std::nullptr_t)
{
    return modelData != nullptr;
}

bool Twin2Engine::GraphicEngine::InstatiatingModel::operator==(const InstatiatingModel& other)
{
    return modelData == other.modelData;
}

bool Twin2Engine::GraphicEngine::InstatiatingModel::operator!=(const InstatiatingModel& other)
{
    return modelData != other.modelData;
}