#include <graphic/InstantiatingModel.h>
#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::GraphicEngine;

//void Twin2Engine::GraphicEngine::InstantiatingModel::setupModel(const std::vector<glm::mat4>& transforms)
//{
//    //for (Mesh& mesh : model->meshes)
//    {
//        //meshes.push_back(InstantiatingMesh(&mesh, transforms));
//    }
//}


//InstantiatingModel::InstantiatingModel(const char* path, const vector<glm::mat4>& transforms)
//{
//    model = new Model(path);
//
//    setupModel(transforms);
//}
Twin2Engine::GraphicEngine::InstantiatingModel::InstantiatingModel(ModelData* modelData)
{
    this->modelData = modelData;
}

Twin2Engine::GraphicEngine::InstantiatingModel::InstantiatingModel(const InstantiatingModel& model)
{
    modelData = model.modelData;
}

Twin2Engine::GraphicEngine::InstantiatingModel::InstantiatingModel(InstantiatingModel&& model)
{
    modelData = model.modelData;
}

Twin2Engine::GraphicEngine::InstantiatingModel::InstantiatingModel()
{
    modelData = nullptr;
}

Twin2Engine::GraphicEngine::InstantiatingModel::~InstantiatingModel() {

}

size_t Twin2Engine::GraphicEngine::InstantiatingModel::GetId() const {
    if (modelData != nullptr) return modelData->id;
    return 0;
}

size_t Twin2Engine::GraphicEngine::InstantiatingModel::GetMeshCount() const
{
    if (modelData == nullptr)
    {
        return 0;
    }

    return modelData->meshes.size();
}

Twin2Engine::GraphicEngine::InstantiatingMesh* Twin2Engine::GraphicEngine::InstantiatingModel::GetMesh(size_t index) const
{
    if (modelData == nullptr)
    {
        return nullptr;
    }

    if (index >= modelData->meshes.size())
    {
        index = modelData->meshes.size() - 1;
    }

    return modelData->meshes[index];
}

//void Twin2Engine::GraphicEngine::InstantiatingModel::Draw(Shader* shaderProgram)
//{
//
//    SPDLOG_ERROR("Not implemented!");
//    //for (unsigned int i = 0; i < meshes.size(); i++)
//    //    meshes[i].Draw(shaderProgram);
//}

Twin2Engine::GraphicEngine::InstantiatingModel& Twin2Engine::GraphicEngine::InstantiatingModel::operator=(const InstantiatingModel& other)
{
    modelData = other.modelData;
    return *this;
}

Twin2Engine::GraphicEngine::InstantiatingModel& Twin2Engine::GraphicEngine::InstantiatingModel::operator=(InstantiatingModel&& other)
{
    modelData = other.modelData;
    return *this;
}

Twin2Engine::GraphicEngine::InstantiatingModel& Twin2Engine::GraphicEngine::InstantiatingModel::operator=(std::nullptr_t)
{
    modelData = nullptr;
    return *this;
}

bool Twin2Engine::GraphicEngine::InstantiatingModel::operator==(std::nullptr_t)
{
    return modelData == nullptr;
}

bool Twin2Engine::GraphicEngine::InstantiatingModel::operator!=(std::nullptr_t)
{
    return modelData != nullptr;
}

bool Twin2Engine::GraphicEngine::InstantiatingModel::operator==(const InstantiatingModel& other)
{
    return modelData == other.modelData;
}

bool Twin2Engine::GraphicEngine::InstantiatingModel::operator!=(const InstantiatingModel& other)
{
    return modelData != other.modelData;
}