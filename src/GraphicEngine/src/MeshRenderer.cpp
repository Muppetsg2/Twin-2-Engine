#include <MeshRenderer.h>

#include <spdlog/spdlog.h>

using namespace GraphicEngine;

void MeshRenderer::ReloadMesh()
{
	//std::cout << "Tutaj9\n";
	MeshRenderingManager::Unregister(this);
	//std::cout << "Tutaj10\n";
	MeshRenderingManager::Register(this);
}

void MeshRenderer::Initialize()
{
	SPDLOG_INFO("Initializing!");
	MeshRenderingManager::Register(this);
	SPDLOG_INFO("EndInitializing!");
}

void MeshRenderer::OnDestroy()
{
	MeshRenderingManager::Unregister(this);
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

size_t GraphicEngine::MeshRenderer::GetMaterialCount() const
{
	return _materials.size();
}

void GraphicEngine::MeshRenderer::AddMaterial(Material material)
{
	_materials.push_back(material);
	ReloadMesh();
}

void GraphicEngine::MeshRenderer::SetMaterial(size_t index, Material material)
{
	if (index < _materials.size())
	{
		_materials[index] = material;
		ReloadMesh();
	}
}

void GraphicEngine::MeshRenderer::SetModel(const InstatiatingModel& model)
{
	_model = model;
	ReloadMesh();
}