#include <graphic/Material.h>
#include <graphic/manager/MaterialsManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

//Material::Material(Shader* shader)
//{
//	_shader = shader;
//}

Material::Material(MaterialData* materialData)
{
	_materialData = materialData;
}

Material::Material(const Material& other)
{
	_materialData = other._materialData;
}

Material::Material(Material&& other)
{
	_materialData = other._materialData;
}

Material::Material(std::nullptr_t)
{
	_materialData = nullptr;
}

Material::Material()
{
	_materialData = nullptr;
}

Material::~Material()
{
	MaterialsManager::UnloadMaterial(*this);
}

Material& Material::operator=(const Material& other)
{
	MaterialsManager::UnloadMaterial(*this);

	_materialData = other._materialData;
	_materialData->useNumber++;

	return *this;
}

Material& Material::operator=(Material&& other)
{
	MaterialsManager::UnloadMaterial(*this);

	_materialData = other._materialData;
	_materialData->useNumber++;

	return *this;
}

Material& Material::operator=(std::nullptr_t)
{
	MaterialsManager::UnloadMaterial(*this);
	
	_materialData = nullptr;

	return *this;
}

bool Material::operator==(std::nullptr_t)
{
	return _materialData == nullptr;
}

bool Material::operator!=(std::nullptr_t)
{
	return _materialData != nullptr;
}

bool Material::operator==(const Material& other)
{
	return _materialData == other._materialData;
}

bool Material::operator!=(const Material& other)
{
	return _materialData != other._materialData;
}

//bool Material::operator<(const Material& other)
//{
//	return _materialData->id < other._materialData->id;
//}

Shader* Twin2Engine::GraphicEngine::Material::GetShader() const
{
	if (_materialData == nullptr)
	{
		return 0;
	}

	return _materialData->shader;
}

unsigned int Twin2Engine::GraphicEngine::Material::GetId() const
{
	if (_materialData == nullptr)
	{
		return 0;
	}

	return _materialData->id;
}

MaterialParameters* Twin2Engine::GraphicEngine::Material::GetMaterialParameters() const
{
	return _materialData->materialParameters;
}

bool Twin2Engine::GraphicEngine::operator<(const Material& material1, const Material& material2)
{
	return material1.GetId() < material2.GetId();
}