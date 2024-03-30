#include <MeshRenderer.h>

using namespace Twin2Engine::GraphicEngine;

void MeshRenderer::ReloadMesh()
{
	//std::cout << "Tutaj9\n";
	MeshRenderingManager::Unregister(this);
	//std::cout << "Tutaj10\n";
	MeshRenderingManager::Register(this);
}

void MeshRenderer::Initialize()
{
	//SPDLOG_INFO("Initializing!");
	MeshRenderingManager::Register(this);
	//SPDLOG_INFO("EndInitializing!");
}

void MeshRenderer::OnDestroy()
{
	MeshRenderingManager::Unregister(this);
}

InstatiatingModel Twin2Engine::GraphicEngine::MeshRenderer::GetModel() const
{
	return _model;
}

size_t Twin2Engine::GraphicEngine::MeshRenderer::GetMeshCount() const
{
	return _model.GetMeshCount();
}

Twin2Engine::GraphicEngine::InstatiatingMesh* Twin2Engine::GraphicEngine::MeshRenderer::GetMesh(size_t index) const
{
	return _model.GetMesh(index);
}

Material Twin2Engine::GraphicEngine::MeshRenderer::GetMaterial(size_t index) const
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

size_t Twin2Engine::GraphicEngine::MeshRenderer::GetMaterialCount() const
{
	return _materials.size();
}

void Twin2Engine::GraphicEngine::MeshRenderer::AddMaterial(Material material)
{
	_materials.push_back(material);
	ReloadMesh();
}

void Twin2Engine::GraphicEngine::MeshRenderer::SetMaterial(size_t index, Material material)
{
	if (index < _materials.size())
	{
		_materials[index] = material;
		ReloadMesh();
	}
}

void Twin2Engine::GraphicEngine::MeshRenderer::SetModel(const InstatiatingModel& model)
{
	_model = model;
	ReloadMesh();
}