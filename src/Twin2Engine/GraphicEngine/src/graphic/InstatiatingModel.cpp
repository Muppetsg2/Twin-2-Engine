#include <graphic/InstatiatingModel.h>
#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::GraphicEngine;

//void Twin2Engine::GraphicEngine::InstatiatingModel::setupModel(const std::vector<glm::mat4>& transforms)
//{
//    //for (Mesh& mesh : model->meshes)
//    {
//        //meshes.push_back(InstatiatingMesh(&mesh, transforms));
//    }
//}


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
}

Twin2Engine::GraphicEngine::InstatiatingModel::InstatiatingModel(InstatiatingModel&& model)
{
    modelData = model.modelData;
}

Twin2Engine::GraphicEngine::InstatiatingModel::InstatiatingModel()
{
    modelData = nullptr;
}

Twin2Engine::GraphicEngine::InstatiatingModel::~InstatiatingModel() {

}

size_t Twin2Engine::GraphicEngine::InstatiatingModel::GetId() const {
    if (modelData != nullptr) return modelData->id;
    return 0;
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

    return modelData->meshes[index];
}

//void Twin2Engine::GraphicEngine::InstatiatingModel::Draw(Shader* shaderProgram)
//{
//
//    SPDLOG_ERROR("Not implemented!");
//    //for (unsigned int i = 0; i < meshes.size(); i++)
//    //    meshes[i].Draw(shaderProgram);
//}

Twin2Engine::GraphicEngine::InstatiatingModel& Twin2Engine::GraphicEngine::InstatiatingModel::operator=(const InstatiatingModel& other)
{
    modelData = other.modelData;
    return *this;
}

Twin2Engine::GraphicEngine::InstatiatingModel& Twin2Engine::GraphicEngine::InstatiatingModel::operator=(InstatiatingModel&& other)
{
    modelData = other.modelData;
    return *this;
}

Twin2Engine::GraphicEngine::InstatiatingModel& Twin2Engine::GraphicEngine::InstatiatingModel::operator=(std::nullptr_t)
{
    modelData = nullptr;
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