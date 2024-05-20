#include <core/MeshRenderer.h>
#include <manager/SceneManager.h>
#include <graphic/manager/ModelsManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <core/CameraComponent.h>


using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

void MeshRenderer::TransformUpdated()
{
	_toUpdate--;
	if (!_toUpdate)
	{
		_transformChanged = false;
	}
}

void MeshRenderer::OnModelDataDestroyed()
{
	if (_loadedModel != 0 && OnTransformChangedActionId != -1) {
		GetTransform()->OnEventTransformChanged -= OnTransformChangedActionId;
	}
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
}

bool MeshRenderer::IsTransformChanged() const
{
	return _transformChanged;
}

void MeshRenderer::Initialize()
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

	OnTransformChangedActionId = GetTransform()->OnEventTransformChanged += [this](Transform* transform) {

		_transformChanged = true;
		_toUpdate = _model.GetMeshCount();
	};
	OnEventInHierarchyParentChangedId = GetTransform()->OnEventInHierarchyParentChanged += [this](Transform* transform) {
		_transformChanged = true;
		_toUpdate = _model.GetMeshCount();
	};
}

void MeshRenderer::Update()
{
	if (ModelsManager::IsModelLoaded(_loadedModel)) OnModelDataDestroyed();
}

YAML::Node MeshRenderer::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["type"] = "MeshRenderer";
	node["model"] = SceneManager::GetModelSaveIdx(_model.GetId());
	node["materials"] = vector<size_t>();
	for (const auto& mat : _materials) {
		node["materials"].push_back(SceneManager::GetMaterialSaveIdx(mat.GetId()));
	}
	return node;
}

bool MeshRenderer::Deserialize(const YAML::Node& node) {
	if (!node["model"] || !node["materials"] ||
		!RenderableComponent::Deserialize(node)) return false;

	_model = ModelsManager::GetModel(SceneManager::GetModel(node["model"].as<size_t>()));
	_materials = vector<Material>();
	for (const auto& mat : node["materials"]) {
		_materials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(mat.as<size_t>())));
	}

	return true;
}

void MeshRenderer::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("Mesh Renderer##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		Component::DrawInheritedFields();
		// TODO: Zrobic
	}
}

InstantiatingModel MeshRenderer::GetModel() const
{
	return _model;
}

size_t MeshRenderer::GetMeshCount() const
{
	return _model.GetMeshCount();
}

InstantiatingMesh* MeshRenderer::GetMesh(size_t index) const
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
	//AddMaterial(MaterialsManager::GetMaterial(materialId));
	_materials.push_back(MaterialsManager::GetMaterial(materialId));
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
	if (_loadedModel != 0 && OnTransformChangedActionId == -1) {
		OnTransformChangedActionId = GetTransform()->OnEventTransformChanged += OnTransformChangedAction;
	}
}

void MeshRenderer::OnDisable()
{
	if (_loadedModel != 0 && OnTransformChangedActionId != -1) {
		GetTransform()->OnEventTransformChanged -= OnTransformChangedActionId;
	}
}

void MeshRenderer::OnDestroy()
{
	if (_loadedModel != 0 && OnTransformChangedActionId != -1) {
		GetTransform()->OnEventTransformChanged -= OnTransformChangedActionId;
	}
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
}
#endif // MESH_FRUSTUM_CULLING

void MeshRenderer::SetModel(const InstantiatingModel& model)
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
		_loadedModel = _model.GetId();

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