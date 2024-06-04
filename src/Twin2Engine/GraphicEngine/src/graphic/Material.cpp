#include <graphic/Material.h>
#include <graphic/manager/MaterialsManager.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

Material::Material(MaterialData materialData)
{
	//_materialData = materialData;

	_id = materialData.id;
	_shader = materialData.shader;
	_materialParameters = materialData.materialParameters;
}

Material::Material(const Material& other)
{
	//_materialData = other._materialData;

	_id = other._id;
	_shader = other._shader;
	_materialParameters = other._materialParameters;
}

Material::Material(Material&& other) noexcept
{
	//_materialData = other._materialData;

	_id = other._id;
	_shader = other._shader;
	_materialParameters = other._materialParameters;
}

Material::Material(std::nullptr_t)
{
	//_materialData = nullptr;

	_id = 0;
	_shader = nullptr;
	_materialParameters = nullptr;
}

Material::Material()
{
	//_materialData = nullptr;

	_id = 0;
	_shader = nullptr;
	_materialParameters = nullptr;
}

Material::~Material()
{
	_shader = nullptr;
	delete _materialParameters;
	_materialParameters = nullptr;
}

/*
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
	_shader = nullptr;
	_materialParameters = nullptr;

	return *this;
}

bool Material::operator==(std::nullptr_t)
{
	return _shader == nullptr || _materialParameters == nullptr;
}

bool Material::operator!=(std::nullptr_t)
{
	return _shader != nullptr && _materialParameters != nullptr;
}
*/

bool Material::operator==(const Material& other)
{
	return _id == other._id && _shader == other._shader && _materialParameters == other._materialParameters;
}

bool Material::operator!=(const Material& other)
{
	return _id != other._id || _shader != other._shader || _materialParameters != other._materialParameters;
}

//bool Material::operator<(const Material& other)
//{
//	return _materialData->id < other._materialData->id;
//}

Shader* Material::GetShader() const
{
	return _shader;
}

size_t Material::GetId() const
{
	return _id;
}

MaterialParameters* Material::GetMaterialParameters() const
{
	return _materialParameters;
}

#if _DEBUG
void Material::DrawEditor() {
	ImGui::Text("Name: ");
	ImGui::SameLine();
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::Text(Twin2Engine::Manager::ShaderManager::GetShaderName(_shader->GetProgramId()).c_str());
	ImGui::PopFont();

	_materialParameters->DrawEditor(_id);
}
#endif

bool Twin2Engine::Graphic::operator<(const Material& material1, const Material& material2)
{
	return material1.GetId() < material2.GetId();
}