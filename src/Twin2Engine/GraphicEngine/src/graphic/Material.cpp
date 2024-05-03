#include <graphic/Material.h>
#include <graphic/manager/MaterialsManager.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

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
}

Material& Material::operator=(const Material& other)
{
	_materialData = other._materialData;
	return *this;
}

Material& Material::operator=(Material&& other)
{
	_materialData = other._materialData;
	return *this;
}

Material& Material::operator=(std::nullptr_t)
{	
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

Shader* Material::GetShader() const
{
	if (_materialData == nullptr)
	{
		return 0;
	}

	return _materialData->shader;
}

size_t Material::GetId() const
{
	if (_materialData == nullptr)
	{
		return 0;
	}

	return _materialData->id;
}

MaterialParameters* Material::GetMaterialParameters() const
{
	return _materialData->materialParameters;
}

bool Twin2Engine::Graphic::operator<(const Material& material1, const Material& material2)
{
	return material1.GetId() < material2.GetId();
}