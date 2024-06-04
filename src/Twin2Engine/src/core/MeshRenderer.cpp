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
			OnStaticChangedId = GetGameObject()->OnStaticChangedEvent += [&](GameObject* g) -> void { OnGameObjectStaticChanged(g); };
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
			GetGameObject()->OnStaticChangedEvent -= OnStaticChangedId;
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

bool MeshRenderer::DrawInheritedFields()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("Mesh Renderer##Component").append(id);

	// Edition Type
	// 0 - move
	// 1 - remove
	static int type = 0;

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::Text("Materials:");
	ImGui::PopFont();

	if (ImGui::RadioButton(std::string("Move##RadioButton").append(id).c_str(), type == 0))
		type = 0;
	ImGui::SameLine();
	if (ImGui::RadioButton(std::string("Remove##RadioButton").append(id).c_str(), type == 1))
		type = 1;

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(string("Items##").append(id).c_str(), node_flag);

	std::list<int> clicked = std::list<int>();
	clicked.clear();
	if (node_open) {
		for (int i = 0; i < _materials.size(); ++i) {
			Material* item = _materials[i];
			size_t num = _addNum[i];
			string n = MaterialsManager::GetMaterialName(item->GetId()).append("##").append(id).append(std::to_string(num));
			ImGui::Text(to_string(i + 1).append(". "s).c_str());
			ImGui::SameLine();
			ImGui::Selectable(n.c_str(), false, NULL, ImVec2(ImGui::GetContentRegionAvail().x - 80, 0.f));

			bool v = false;
			if (type == 0) v = ImGui::IsItemActive() && !ImGui::IsItemHovered();

			if (type == 1) {
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
				if (ImGui::RemoveButton(string("##Remove").append(id).append(std::to_string(i)).c_str())) {
					clicked.push_back(i);
				}
			}

			if (type == 0 && v)
			{
				size_t i_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
				if (i_next >= 0 && i_next < _materials.size())
				{
					_addNum[i] = _addNum[i_next];
					SetMaterial(i, _materials[i_next]);

					_addNum[i_next] = num;
					SetMaterial(i_next, item);

					ImGui::ResetMouseDragDelta();
				}
			}
		}
		ImGui::TreePop();
	}

	if (clicked.size() > 0 && type == 1) {
		clicked.sort();

		for (int i = clicked.size() - 1; i > -1; --i)
		{
			RemoveMaterial(clicked.back());

			clicked.pop_back();
		}
	}

	clicked.clear();

	if (ImGui::Button(string("Add Material##").append(id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		ImGui::OpenPopup(string("Add Material PopUp##Mesh Renderer").append(id).c_str(), ImGuiPopupFlags_NoReopen);
	}

	if (ImGui::BeginPopup(string("Add Material PopUp##Mesh Renderer").append(id).c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {

		std::map<size_t, std::string> types = MaterialsManager::GetAllMaterialsNames();

		size_t choosed = 0;

		if (ImGui::BeginCombo(string("##COMPONENT POP UP MATERIALS").append(id).c_str(), choosed == 0 ? "None" : types[choosed].c_str())) {

			bool click = false;
			size_t i = 0;
			for (auto& item : types) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).append(std::to_string(i)).c_str(), item.first == choosed)) {

					if (click) continue;

					choosed = item.first;
					click = true;
				}

				++i;
			}

			if (click) {
				if (choosed != 0) {
					AddMaterial(choosed);
				}
			}

			ImGui::EndCombo();
		}

		if (choosed != 0) {
			ImGui::CloseCurrentPopup();
		}

		types.clear();

		ImGui::EndPopup();
	}

	return false;
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
	if (_materialsErasedEventDone) return;
	Unregister();
	_materialsErasedEventDone = true;
}

bool MeshRenderer::CheckMaterialsValidation()
{
	bool res = true;
	for (size_t i = _materials.size(); i > 0; --i) {
		if (!MaterialsManager::IsMaterialLoaded(_materials[i - 1]->GetId())) {
			res = false;
			_materialError = true;
			Unregister();
			_materials.erase(_materials.begin() + i - 1);

#if _DEBUG
			_next = _next > _addNum[i - 1] ? _addNum[i - 1] : _next;
			_addNum.erase(_addNum.begin() + i - 1);
#endif

		}
	}

	Register();

	return res;
}

void MeshRenderer::TransformUpdated()
{
	_meshesToUpdate--;
	if (!_meshesToUpdate)
	{
		_transformChanged = false;
	}
}

void MeshRenderer::SetIsTransparent(bool value)
{
	if (_isTransparent != value) {
		Unregister();
		_isTransparent = value;
		Register();
		OnTransparentChangedEvent.Invoke(this);
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

	if (_materials.size() != 0) {

		if (_materialsErasedEventDone) _materialsErasedEventDone = false;

		if (!CheckMaterialsValidation()) {
			if (_materialError) _materialError = false;
			if (_materials.size() == 0) OnMaterialsErased();
		}
	}
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

#if _DEBUG
	_addNum.clear();
#endif
}

YAML::Node MeshRenderer::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["type"] = "MeshRenderer";
	node["model"] = SceneManager::GetModelSaveIdx(_model.GetId());
	node["materials"] = vector<size_t>();
	for (const auto& mat : _materials) {
		node["materials"].push_back(SceneManager::GetMaterialSaveIdx(mat->GetId()));
	}
	return node;
}

bool MeshRenderer::Deserialize(const YAML::Node& node) {
	if (!node["model"] || 
		!node["materials"] ||
		!RenderableComponent::Deserialize(node)) return false;

	_model = ModelsManager::GetModel(SceneManager::GetModel(node["model"].as<size_t>()));
	_loadedModel = _model.GetId();

	_materials = vector<Material*>();
	for (const auto& mat : node["materials"]) {
#if _DEBUG
		_addNum.push_back(_next);

		++_next;

		while (std::find(_addNum.begin(), _addNum.end(), _next) != _addNum.end()) ++_next;
#endif
		_materials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(mat.as<size_t>())));
	}

	return true;
}

#if _DEBUG
void MeshRenderer::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("Mesh Renderer##Component").append(id);

	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;
		ImGui::Checkbox(string("Transparent##").append(id).c_str(), &_isTransparent);

		std::map<size_t, string> modelNames = ModelsManager::GetAllModelsNames();

		modelNames.insert(std::pair(0, "None"));

		if (!modelNames.contains(_model.GetId())) {
			SetModel(InstantiatingModel());
		}

		if (ImGui::BeginCombo(string("Model##").append(id).c_str(), modelNames[_loadedModel].c_str())) {

			bool clicked = false;
			size_t choosed = _loadedModel;
			for (auto& item : modelNames) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).c_str(), item.first == _loadedModel)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed != 0) {
					SetModel(choosed);
				}
				else {
					SetModel(InstantiatingModel());
				}
			}

			ImGui::EndCombo();
		}

		DrawInheritedFields();
	}
}
#endif

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

bool MeshRenderer::IsMaterialError() const {
	return _materialError;
}

size_t MeshRenderer::GetMaterialCount() const
{
	return _materials.size();
}

Material* MeshRenderer::GetMaterial(size_t index) const
{
	if (_materials.size() == 0) return nullptr;

	if (index >= _materials.size())
	{
		SPDLOG_WARN("MESH_RENDERER::Trying to acces material out of bounds.");
		return _materials[_materials.size() - 1];
	}

	return _materials[index];
}

void MeshRenderer::AddMaterial(Material* material)
{
	if (material == nullptr) return;
	Unregister();

#if _DEBUG
	_addNum.push_back(_next);

	++_next;

	while (std::find(_addNum.begin(), _addNum.end(), _next) != _addNum.end()) ++_next;
#endif

	_materials.push_back(material);

	if (_materials.size() != 0 && _loadedModel != 0) Register();
}

void MeshRenderer::AddMaterial(size_t materialId)
{
	AddMaterial(MaterialsManager::GetMaterial(materialId));
}

void MeshRenderer::SetMaterial(size_t index, Material* material)
{
	if (_materials[index] == material || index >= _materials.size() || material == nullptr) return;

	Unregister();

	_materials[index] = material;

	if (_loadedModel != 0) Register();
}

void MeshRenderer::SetMaterial(size_t index, size_t materialId)
{
	SetMaterial(index, MaterialsManager::GetMaterial(materialId));
}

void MeshRenderer::RemoveMaterial(size_t index)
{
	if (index >= _materials.size()) return;

	Unregister();

#if _DEBUG
	_next = _addNum[index];
	_addNum.erase(_addNum.begin() + index);
#endif
	_materials.erase(_materials.begin() + index);

	if (_materials.size() != 0 && _loadedModel != 0) Register();
}