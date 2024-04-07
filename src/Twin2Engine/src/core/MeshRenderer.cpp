#include <core/MeshRenderer.h>


using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

void MeshRenderer::Render()
{
	MeshRenderData data{};
	data.transform = GetTransform()->GetTransformMatrix();
	data.meshes = std::vector<InstatiatingMesh*>();
	data.materials = std::vector<Material>();
	for (size_t i = 0; i < _model.GetMeshCount(); ++i) {
		data.meshes.push_back(_model.GetMesh(i));

		//if (i >= _materials.size())
		//	data.materials.push_back(nullptr);
		//else
		//	data.materials.push_back(_materials[i]);
		data.materials.push_back(GetMaterial(i));
	}
	MeshRenderingManager::Render(data);
}

InstatiatingModel MeshRenderer::GetModel() const
{
	return _model;
}

size_t MeshRenderer::GetMeshCount() const
{
	return _model.GetMeshCount();
}

InstatiatingMesh* MeshRenderer::GetMesh(size_t index) const
{
	return _model.GetMesh(index);
}

Material MeshRenderer::GetMaterial(size_t index) const
{
	if (_materials.size() == 0)
	{
		return nullptr;
	}

	if (index >= _materials.size())
	{
		index = _materials.size() - 1;
	}

	return _materials[index];
}

size_t MeshRenderer::GetMaterialCount() const
{
	return _materials.size();
}

void MeshRenderer::AddMaterial(Material material)
{
	_materials.push_back(material);
}

void MeshRenderer::SetMaterial(size_t index, Material material)
{
	if (index < _materials.size())
	{
		_materials[index] = material;
	}
}

void MeshRenderer::SetModel(const InstatiatingModel& model)
{
	_model = model;
}