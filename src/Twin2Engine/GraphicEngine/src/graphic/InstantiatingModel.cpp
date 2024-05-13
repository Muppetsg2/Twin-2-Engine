#include <graphic/InstantiatingModel.h>
#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::Graphic;

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
Twin2Engine::Graphic::InstantiatingModel::InstantiatingModel(ModelData* modelData)
{
    this->modelData = modelData;
}

Twin2Engine::Graphic::InstantiatingModel::InstantiatingModel(const InstantiatingModel& model)
{
    modelData = model.modelData;
}

Twin2Engine::Graphic::InstantiatingModel::InstantiatingModel(InstantiatingModel&& model)
{
    modelData = model.modelData;
}

Twin2Engine::Graphic::InstantiatingModel::InstantiatingModel()
{
    modelData = nullptr;
}

Twin2Engine::Graphic::InstantiatingModel::~InstantiatingModel() {

}

size_t Twin2Engine::Graphic::InstantiatingModel::GetId() const {
    if (modelData != nullptr) return modelData->id;
    return 0;
}

size_t Twin2Engine::Graphic::InstantiatingModel::GetMeshCount() const
{
    if (modelData == nullptr)
    {
        return 0;
    }

    return modelData->meshes.size();
}

Twin2Engine::Graphic::InstantiatingMesh* Twin2Engine::Graphic::InstantiatingModel::GetMesh(size_t index) const
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

Twin2Engine::Graphic::InstantiatingModel& Twin2Engine::Graphic::InstantiatingModel::operator=(const InstantiatingModel& other)
{
    modelData = other.modelData;
    return *this;
}

Twin2Engine::Graphic::InstantiatingModel& Twin2Engine::Graphic::InstantiatingModel::operator=(InstantiatingModel&& other)
{
    modelData = other.modelData;
    return *this;
}

Twin2Engine::Graphic::InstantiatingModel& Twin2Engine::Graphic::InstantiatingModel::operator=(std::nullptr_t)
{
    modelData = nullptr;
    return *this;
}

bool Twin2Engine::Graphic::InstantiatingModel::operator==(std::nullptr_t)
{
    return modelData == nullptr;
}

bool Twin2Engine::Graphic::InstantiatingModel::operator!=(std::nullptr_t)
{
    return modelData != nullptr;
}

bool Twin2Engine::Graphic::InstantiatingModel::operator==(const InstantiatingModel& other)
{
    return modelData == other.modelData;
}

bool Twin2Engine::Graphic::InstantiatingModel::operator!=(const InstantiatingModel& other)
{
    return modelData != other.modelData;
}