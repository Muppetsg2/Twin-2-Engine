#include <core/MeshRenderer.h>
#include <manager/SceneManager.h>
#include <graphic/manager/ModelsManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <core/CameraComponent.h>


using namespace Twin2Engine::Core;
using namespace Twin2Engine::Tools;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

void Twin2Engine::Core::MeshRenderer::TransformUpdated()
{
	_toUpdate--;
	if (!_toUpdate)
	{
		_transformChanged = false;
	}
}

bool Twin2Engine::Core::MeshRenderer::IsTransformChanged() const
{
	return _transformChanged;
}


void Twin2Engine::Core::MeshRenderer::Initialize()
{
	GetGameObject()->OnStaticChanged += [this](GameObject* gameObject) {
		if (gameObject->GetIsStatic())
		{
			if (_registered)
			{
				MeshRenderingManager::UnregisterDynamic(this);
			}
			MeshRenderingManager::RegisterStatic(this);
			_registered = true;
		}
		else
		{
			if (_registered)
			{
				MeshRenderingManager::UnregisterStatic(this);
			}
			MeshRenderingManager::RegisterDynamic(this);
			_registered = true;
		}
		};

	_transformChanged = false;
	if (GetGameObject()->GetIsStatic())
	{
		if (_registered)
		{
			MeshRenderingManager::UnregisterDynamic(this);
		}
		MeshRenderingManager::RegisterStatic(this);
	}
	else
	{
		if (_registered)
		{
			MeshRenderingManager::UnregisterStatic(this);
		}
		MeshRenderingManager::RegisterDynamic(this);
	}
	_registered = true;

	GetTransform()->OnEventTransformChanged += [this](Transform* transform) {
		_transformChanged = true;
		_toUpdate = _model.GetMeshCount();
		};
	GetTransform()->OnEventInHierarchyParentChanged += [this](Transform* transform) {
		_transformChanged = true;
		_toUpdate = _model.GetMeshCount();
		};
}

void MeshRenderer::Render()
{
	//if (!GetGameObject()->GetIsStatic())
	//{
	//	MeshRenderData data{};
	//	data.transform = GetTransform()->GetTransformMatrix();
	//	data.meshes = std::vector<InstatiatingMesh*>();
	//	data.materials = std::vector<Material>();
	//	data.isTransparent = IsTransparent();
	//
	//
	//	if (CameraComponent::GetMainCamera()->IsFrustumCullingOn)
	//	{
	//		InstatiatingMesh* tMesh;
	//		Frustum frustum = CameraComponent::GetMainCamera()->GetFrustum();
	//		for (size_t i = 0; i < _model.GetMeshCount(); ++i) {
	//			tMesh = _model.GetMesh(i);
	//			if (tMesh->IsOnFrustum(frustum, data.transform)) {
	//				data.meshes.push_back(tMesh);
	//				data.materials.push_back(GetMaterial(i));
	//			}
	//		}
	//	}
	//	else
	//	{
	//		for (size_t i = 0; i < _model.GetMeshCount(); ++i) {
	//			data.meshes.push_back(_model.GetMesh(i));
	//			data.materials.push_back(GetMaterial(i));
	//		}
	//	}
	//
	//	MeshRenderingManager::Render(data);
	//}
}

YAML::Node MeshRenderer::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<string>());
	node["type"] = "MeshRenderer";
	node["model"] = SceneManager::GetModelSaveIdx(_model.GetId());
	node["materials"] = vector<size_t>();
	for (const auto& mat : _materials) {
		node["materials"].push_back(SceneManager::GetMaterialSaveIdx(mat.GetId()));
	}
	return node;
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
	if (_registered)
	{
		if (GetGameObject()->GetIsStatic())
		{
			MeshRenderingManager::UnregisterStatic(this);
		}
		else
		{
			MeshRenderingManager::UnregisterDynamic(this);
		}
	}

	_materials.push_back(material);

	if (_registered)
	{
		if (GetGameObject()->GetIsStatic())
		{
			MeshRenderingManager::RegisterStatic(this);
		}
		else
		{
			MeshRenderingManager::RegisterDynamic(this);
		}
	}
	//_registered = true;
}

void MeshRenderer::AddMaterial(size_t materialId)
{
	AddMaterial(MaterialsManager::GetMaterial(materialId));
}

void MeshRenderer::SetMaterial(size_t index, Material material)
{
	if (_materials[index] != material && index < _materials.size())
	{
		if (_registered)
		{
			if (GetGameObject()->GetIsStatic())
			{
				MeshRenderingManager::UnregisterStatic(this);
			}
			else
			{
				MeshRenderingManager::UnregisterDynamic(this);
			}
		}

		_materials[index] = material;

		if (_registered)
		{
			if (GetGameObject()->GetIsStatic())
			{
				MeshRenderingManager::RegisterStatic(this);
			}
			else
			{
				MeshRenderingManager::RegisterDynamic(this);
			}
		}
		//_registered = true;
	}
}

void MeshRenderer::SetMaterial(size_t index, size_t materialId)
{
	SetMaterial(index, MaterialsManager::GetMaterial(materialId));
}

#ifdef MESH_FRUSTUM_CULLING
void MeshRenderer::OnEnable()
{
	if (_model != nullptr && OnTransformChangedActionId == -1) {
		OnTransformChangedActionId = GetTransform()->OnEventTransformChanged += OnTransformChangedAction;
	}
}

void MeshRenderer::OnDisable()
{
	if (_model != nullptr && OnTransformChangedActionId != -1) {
		GetTransform()->OnEventTransformChanged -= OnTransformChangedActionId;
	}
}

void MeshRenderer::OnDestroy()
{
	if (_model != nullptr && OnTransformChangedActionId != -1) {
		GetTransform()->OnEventTransformChanged -= OnTransformChangedActionId;
	}
}
#endif // MESH_FRUSTUM_CULLING

void MeshRenderer::SetModel(const InstatiatingModel& model)
{
	if (_model != model)
	{
		if (_registered)
		{
			if (GetGameObject()->GetIsStatic())
			{
				MeshRenderingManager::UnregisterStatic(this);
			}
			else
			{
				MeshRenderingManager::UnregisterDynamic(this);
			}
		}

		_model = model;

		if (_registered)
		{
			if (GetGameObject()->GetIsStatic())
			{
				MeshRenderingManager::RegisterStatic(this);
			}
			else
			{
				MeshRenderingManager::RegisterDynamic(this);
			}
		}
		//_registered = true;

#ifdef MESH_FRUSTUM_CULLING
		if (OnTransformChangedActionId == -1 && GetGameObject() != nullptr) {
			OnTransformChangedActionId = GetTransform()->OnEventTransformChanged += OnTransformChangedAction;
		}
#endif // MESH_FRUSTUM_CULLING

		if (_transformChanged)
		{
			_toUpdate = _model.GetMeshCount();
		}
	}
}

void MeshRenderer::SetModel(size_t modelId)
{
	SetModel(ModelsManager::GetModel(modelId));
}