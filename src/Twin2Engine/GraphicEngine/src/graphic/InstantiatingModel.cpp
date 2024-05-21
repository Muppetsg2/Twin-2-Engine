#include <graphic/InstantiatingModel.h>
#include <graphic/manager/ModelsManager.h>

using namespace Twin2Engine::Graphic;

InstantiatingModel::InstantiatingModel(ModelData* modelData)
{
    this->modelData = modelData;
}

InstantiatingModel::InstantiatingModel(const InstantiatingModel& model)
{
    modelData = model.modelData;
}

InstantiatingModel::InstantiatingModel(InstantiatingModel&& model)
{
    modelData = model.modelData;
}

InstantiatingModel::InstantiatingModel()
{
    modelData = nullptr;
}

InstantiatingModel::~InstantiatingModel() {
    modelData = nullptr;
}

size_t InstantiatingModel::GetId() const {
    if (modelData != nullptr) return modelData->id;
    return 0;
}

size_t InstantiatingModel::GetMeshCount() const
{
    if (modelData == nullptr) return 0;

    return modelData->meshes.size();
}

InstantiatingMesh* InstantiatingModel::GetMesh(size_t index) const
{
    if (modelData == nullptr) return nullptr;

    if (index >= modelData->meshes.size())
    {
        index = modelData->meshes.size() - 1;
    }

    return modelData->meshes[index];
}

InstantiatingModel& InstantiatingModel::operator=(const InstantiatingModel& other)
{
    modelData = other.modelData;
    return *this;
}

InstantiatingModel& InstantiatingModel::operator=(InstantiatingModel&& other)
{
    modelData = other.modelData;
    return *this;
}

InstantiatingModel& InstantiatingModel::operator=(std::nullptr_t)
{
    modelData = nullptr;
    return *this;
}

bool InstantiatingModel::operator==(std::nullptr_t)
{
    return modelData == nullptr;
}

bool InstantiatingModel::operator!=(std::nullptr_t)
{
    return modelData != nullptr;
}

bool InstantiatingModel::operator==(const InstantiatingModel& other)
{
    return modelData == other.modelData;
}

bool InstantiatingModel::operator!=(const InstantiatingModel& other)
{
    return modelData != other.modelData;
}