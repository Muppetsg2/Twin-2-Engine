#include <MeshRenderer.h>

using namespace GraphicEngine;

void MeshRenderer::Initialize()
{
	//MeshRenderingManager::Register(this);
}

void MeshRenderer::OnDestroy()
{
	//MeshRenderingManager::Unregister(this);
}

InstatiatingModel GraphicEngine::MeshRenderer::GetModel() const
{
	return _model;
}

size_t GraphicEngine::MeshRenderer::GetMeshCount() const
{
	return _model.GetMeshCount();
}

GraphicEngine::InstatiatingMesh* GraphicEngine::MeshRenderer::GetMesh(size_t index) const
{
	return _model.GetMesh(index);
}

Material GraphicEngine::MeshRenderer::GetMaterial(size_t index) const
{
	if (_materials.size())
	{
		return nullptr;
	}

	if (index >= _materials.size())
	{
		index = _materials.size() - 1;
	}

	return _materials[index];
}

size_t GraphicEngine::MeshRenderer::GetMaterialCount() const
{
	return _materials.size();
}

void GraphicEngine::MeshRenderer::AddMaterial(Material material)
{
	_materials.push_back(material);
}

void GraphicEngine::MeshRenderer::SetMaterial(size_t index, Material material)
{
	if (index < _materials.size())
	{
		_materials[index] = material;
	}
}

void GraphicEngine::MeshRenderer::SetModel(const InstatiatingModel& model)
{
	_model = model;
}