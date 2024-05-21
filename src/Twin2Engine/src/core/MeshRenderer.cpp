#include <core/MeshRenderer.h>
#include <manager/SceneManager.h>
#include <graphic/manager/ModelsManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <core/CameraComponent.h>
#include <graphic/InstantiatingModel.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

void MeshRenderer::Register()
{
	if (_registered) return;

	bool res = false;

	if (GetGameObject()->GetIsStatic())
	{
		res = MeshRenderingManager::RegisterStatic(this);
	}
	else
	{
		res = MeshRenderingManager::RegisterDynamic(this);
	}

	if (res) {
		// EVENTS
		if (OnStaticChangedId == -1) {
			OnStaticChangedId = GetGameObject()->OnStaticChanged += [&](GameObject* g) -> void { OnGameObjectStaticChanged(g); };
		}

		if (OnTransformMatrixChangedId == -1) {
			OnTransformMatrixChangedId = GetTransform()->OnEventTransformChanged += [&](Transform* t) -> void { OnTransformMatrixChanged(t); };
		}

		if (OnEventInHierarchyParentChangedId == -1) {
			OnEventInHierarchyParentChangedId = GetTransform()->OnEventInHierarchyParentChanged += [&](Transform* t) -> void { OnTransformMatrixChanged(t); };
		}
	}

	_registered = res;
}

void MeshRenderer::Unregister()
{
	if (!_registered) return;

	bool res = false;

	if (GetGameObject()->GetIsStatic())
	{
		res = MeshRenderingManager::UnregisterStatic(this);
	}
	else
	{
		res = MeshRenderingManager::UnregisterDynamic(this);
	}

	if (res) {
		// EVENTS
		if (OnStaticChangedId != -1) {
			GetGameObject()->OnStaticChanged -= OnStaticChangedId;
			OnStaticChangedId = -1;
		}

		if (OnTransformMatrixChangedId != -1) {
			GetTransform()->OnEventTransformChanged -= OnTransformMatrixChangedId;
			OnTransformMatrixChangedId = -1;
		}

		if (OnEventInHierarchyParentChangedId != -1) {
			GetTransform()->OnEventInHierarchyParentChanged -= OnEventInHierarchyParentChangedId;
			OnEventInHierarchyParentChangedId = -1;
		}
	}

	_registered = !res;
}

void MeshRenderer::OnModelDataDestroyed()
{
	if (_loadedModel == 0) return;
	Unregister();
	_meshesToUpdate = 0;
	_transformChanged = false;
	_loadedModel = 0;
	_model = InstantiatingModel();
}

void MeshRenderer::OnGameObjectStaticChanged(GameObject* gameObject)
{
	if (gameObject->GetIsStatic())
	{
		bool res = true;
		if (_registered)
		{
			res = MeshRenderingManager::UnregisterDynamic(this);
		}

		if (!res) {
			res = MeshRenderingManager::UnregisterStatic(this);
		}

		res = MeshRenderingManager::RegisterStatic(this);
		_registered = res;
	}
	else
	{
		bool res = true;
		if (_registered)
		{
			res = MeshRenderingManager::UnregisterStatic(this);
		}

		if (!res) {
			res = MeshRenderingManager::UnregisterDynamic(this);
		}

		res = MeshRenderingManager::RegisterDynamic(this);
		_registered = res;
	}
}

void MeshRenderer::OnTransformMatrixChanged(Transform* transform)
{
	if (_loadedModel != 0) {
		_transformChanged = true;
		_meshesToUpdate = _model.GetMeshCount();

#ifdef MESH_FRUSTUM_CULLING
		glm::mat4 tMatrix = transform->GetTransformMatrix();

		for (size_t i = 0; i < _model.GetMeshCount(); ++i) {
			Physic::SphereColliderData* sphereBV = (Physic::SphereColliderData*)_model.GetMesh(i)->sphericalBV->colliderShape;
			if (sphereBV != nullptr) {
				sphereBV->Position = tMatrix * glm::vec4(sphereBV->LocalPosition, 1.0f);
			}
			sphereBV = nullptr;
		}
#endif
	}
}

void MeshRenderer::OnMaterialsErased()
{
	if (materialsErasedEventDone) return;
	Unregister();
	materialsErasedEventDone = true;
}

void MeshRenderer::TransformUpdated()
{
	_meshesToUpdate--;
	if (!_meshesToUpdate)
	{
		_transformChanged = false;
	}
}

void MeshRenderer::Initialize()
{
	_transformChanged = false;

	if (_loadedModel != 0) {
		if (_materials.size() != 0) Register();
		_transformChanged = true;
		_meshesToUpdate = _model.GetMeshCount();
	}
}

void MeshRenderer::Update()
{
	if (!ModelsManager::IsModelLoaded(_loadedModel)) OnModelDataDestroyed();

	if (_materials.size() == 0) OnMaterialsErased();

	if (_materials.size() != 0 && materialsErasedEventDone) materialsErasedEventDone = false;
}

void MeshRenderer::OnEnable()
{
	if (_loadedModel != 0) {
		if (_materials.size() != 0) Register();
		_transformChanged = true;
		_meshesToUpdate = _model.GetMeshCount();
	}
}

void MeshRenderer::OnDisable()
{
	_transformChanged = false;
	_meshesToUpdate = 0;
	Unregister();
}

void MeshRenderer::OnDestroy()
{
	Unregister();
	_meshesToUpdate = 0;
	_transformChanged = false;
	_loadedModel = 0;
	_model = InstantiatingModel();
	_materials.clear();
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
	if (!node["model"] || 
		!node["materials"] ||
		!RenderableComponent::Deserialize(node)) return false;

	_model = ModelsManager::GetModel(SceneManager::GetModel(node["model"].as<size_t>()));
	_loadedModel = _model.GetId();

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

bool MeshRenderer::IsTransformChanged() const
{
	return _transformChanged;
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

void MeshRenderer::SetModel(const InstantiatingModel& model)
{
	if (_loadedModel == model.GetId()) return;

	Unregister();

	_model = model;
	_loadedModel = _model.GetId();
	_transformChanged = true;
	_meshesToUpdate = _model.GetMeshCount();

	if (_materials.size() != 0 && _loadedModel != 0) Register();
}

void MeshRenderer::SetModel(size_t modelId)
{
	SetModel(ModelsManager::GetModel(modelId));
}

size_t MeshRenderer::GetMaterialCount() const
{
	return _materials.size();
}

Material MeshRenderer::GetMaterial(size_t index) const
{
	if (_materials.size() == 0) return nullptr;

	if (index >= _materials.size())
	{
		SPDLOG_WARN("MESH_RENDERER::Trying to acces material out of bounds.");
		return _materials[_materials.size() - 1];
	}

	return _materials[index];
}

void MeshRenderer::AddMaterial(Material material)
{
	Unregister();

	_materials.push_back(material);

	if (_loadedModel != 0) Register();
}

void MeshRenderer::AddMaterial(size_t materialId)
{
	_materials.push_back(MaterialsManager::GetMaterial(materialId));
}

void MeshRenderer::SetMaterial(size_t index, Material material)
{
	if (_materials[index] == material || index >= _materials.size()) return;

	Unregister();

	_materials[index] = material;

	if (_loadedModel != 0) Register();
}

void MeshRenderer::SetMaterial(size_t index, size_t materialId)
{
	SetMaterial(index, MaterialsManager::GetMaterial(materialId));
}