#include <InstatiatingModel.h>

#include <ModelsManager.h>

#include <spdlog/spdlog.h>

using namespace GraphicEngine;

void GraphicEngine::InstatiatingModel::setupModel(const vector<glm::mat4>& transforms)
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
GraphicEngine::InstatiatingModel::InstatiatingModel(ModelData* modelData)
{
    this->modelData = modelData;
}

GraphicEngine::InstatiatingModel::InstatiatingModel(const InstatiatingModel& model)
{
    modelData = model.modelData;

    modelData->useNumber++;
}

GraphicEngine::InstatiatingModel::InstatiatingModel(InstatiatingModel&& model)
{
    modelData = model.modelData;
    modelData->useNumber++;

}

GraphicEngine::InstatiatingModel::InstatiatingModel()
{
    modelData = nullptr;
}

GraphicEngine::InstatiatingModel::~InstatiatingModel()
{
    GraphicEngine::ModelsManager::UnloadModel(modelData);
}

size_t GraphicEngine::InstatiatingModel::GetMeshCount() const
{
    return modelData->meshes.size();
}

InstatiatingMesh* GraphicEngine::InstatiatingModel::GetMesh(size_t index) const
{
    if (index >= modelData->meshes.size())
    {
        index = modelData->meshes.size() - 1;
    }

    return &(modelData->meshes[index]);
}

void GraphicEngine::InstatiatingModel::Draw(Shader* shaderProgram)
{

    SPDLOG_ERROR("Not implemented!");
    //for (unsigned int i = 0; i < meshes.size(); i++)
    //    meshes[i].Draw(shaderProgram);
}

void GraphicEngine::InstatiatingModel::SetTransform(unsigned int meshIndex, const vector<Transform*> transforms)
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

InstatiatingModel& InstatiatingModel::operator=(const InstatiatingModel& other)
{
    GraphicEngine::ModelsManager::UnloadModel(modelData);
    modelData = other.modelData;
    modelData->useNumber++;
    return *this;
}

InstatiatingModel& InstatiatingModel::operator=(InstatiatingModel&& other)
{
    GraphicEngine::ModelsManager::UnloadModel(modelData);
    modelData = other.modelData;
    modelData->useNumber++;

    return *this;
}

InstatiatingModel& InstatiatingModel::operator=(std::nullptr_t)
{
    GraphicEngine::ModelsManager::UnloadModel(modelData);

    return *this;
}

bool InstatiatingModel::operator==(std::nullptr_t)
{
    return modelData == nullptr;
}

bool InstatiatingModel::operator!=(std::nullptr_t)
{
    return modelData != nullptr;
}

bool InstatiatingModel::operator==(const InstatiatingModel& other)
{
    return modelData == other.modelData;
}

bool InstatiatingModel::operator!=(const InstatiatingModel& other)
{
    return modelData != other.modelData;
}