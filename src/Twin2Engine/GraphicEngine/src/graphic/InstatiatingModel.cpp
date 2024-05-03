#include <graphic/InstatiatingModel.h>
#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::Graphic;

InstatiatingModel::InstatiatingModel(ModelData* modelData)
{
    this->modelData = modelData;
}

InstatiatingModel::InstatiatingModel(const InstatiatingModel& model)
{
    modelData = model.modelData;
}

InstatiatingModel::InstatiatingModel(InstatiatingModel&& model)
{
    modelData = model.modelData;
}

InstatiatingModel::InstatiatingModel()
{
    modelData = nullptr;
}

InstatiatingModel::~InstatiatingModel() {

}

size_t InstatiatingModel::GetId() const {
    if (modelData != nullptr) return modelData->id;
    return 0;
}

size_t InstatiatingModel::GetMeshCount() const
{
    if (modelData == nullptr)
    {
        return 0;
    }

    return modelData->meshes.size();
}

InstatiatingMesh* InstatiatingModel::GetMesh(size_t index) const
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

InstatiatingModel& InstatiatingModel::operator=(const InstatiatingModel& other)
{
    modelData = other.modelData;
    return *this;
}

InstatiatingModel& InstatiatingModel::operator=(InstatiatingModel&& other)
{
    modelData = other.modelData;
    return *this;
}

InstatiatingModel& InstatiatingModel::operator=(std::nullptr_t)
{
    modelData = nullptr;
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