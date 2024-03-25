#include <Material.h>

using namespace GraphicEngine;

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

Shader* GraphicEngine::Material::GetShader() const
{
	if (_materialData == nullptr)
	{
		return 0;
	}

	return _materialData->shader;
}

unsigned int GraphicEngine::Material::GetId() const
{
	if (_materialData == nullptr)
	{
		return 0;
	}

	return _materialData->id;
}


bool GraphicEngine::operator<(const Material& material1, const Material& material2)
{
	return material1.GetId() < material2.GetId();
}