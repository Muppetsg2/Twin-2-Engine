#include <core/GameObject.h>
#include <core/ComponentsMap.h>
#include <manager/SceneManager.h>
#include <manager/PrefabManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

size_t Twin2Engine::Core::GameObject::_currentFreeId = 1;
vector<size_t> Twin2Engine::Core::GameObject::_freedIds;
std::unordered_set<std::string_view> Twin2Engine::Core::GameObject::AllTags;

GameObject* GameObject::Clone() const
{
	GameObject* cloned = new GameObject();

	CloneTo(cloned);

	return cloned;
}

void GameObject::CloneTo(GameObject* cloned) const
{
	cloned->_activeSelf = _activeSelf;
	cloned->_isStatic = _isStatic;
	cloned->_name = _name;

	cloned->_transform->SetLocalPosition(_transform->GetLocalPosition());
	cloned->_transform->SetLocalRotation(_transform->GetLocalRotation());
	cloned->_transform->SetLocalScale(_transform->GetLocalScale());

	// Pomijanie Transforma z listy komponent�w
	auto component = std::next(components.begin());

	for (; component != components.end(); ++component)
	{
		Component* clonedComponent = (*component)->Clone();
		clonedComponent->Init(cloned);
		cloned->components.push_back(clonedComponent);
	}

	for (int index = 0; index < _transform->GetChildCount(); ++index)
	{
		//cloned->_transform->AddChild(Instantiate(_transform->GetChildAt(index)->GetGameObject(), cloned->_transform)->GetTransform());
		Instantiate(_transform->GetChildAt(index)->GetGameObject(), cloned->_transform);
	}
}

size_t GameObject::GetFreeId()
{
	size_t id;
	if (_freedIds.size() > 0) {
		id = _freedIds.front();
		for (size_t i = 0; i < _freedIds.size() - 1; ++i) {
			_freedIds[i] = _freedIds[i + 1];
		}
		_freedIds.resize(_freedIds.size() - 1);
	}
	else {
		id = _currentFreeId++;
	}
	return id;
}

void GameObject::FreeId(size_t id)
{
	if (_currentFreeId == id + 1) {
		--_currentFreeId;

		if (_freedIds.size() > 0) {
			while (_currentFreeId == _freedIds.back() + 1) {
				_freedIds.pop_back();
				--_currentFreeId;

				if (_freedIds.size() == 0) {
					break;
				}
			}
		}
	}
	else {
		_freedIds.push_back(id);
		sort(_freedIds.begin(), _freedIds.end());
	}
}

void GameObject::SetActiveInHierarchy(bool activeInHierarchy)
{
	if (_activeInHierarchy != activeInHierarchy) // warunek sprawdzajacy czy to ustawienie zmieni stan (musi zmieniac inaczej nie ma sensu dzialac dalej)
	{
		_activeInHierarchy = activeInHierarchy; //zmiana stanu
		if (_activeSelf) // sprawdzenie wlasnego stanu, jezeli ustawiony na false to znaczy, ze ten stan dyktuje warunki aktywnosci wszystkich podrzednych obiektow
		{
			for (int index = 0; index < _transform->GetChildCount(); ++index)
			{
				_transform->GetChildAt(index)->GetGameObject()->SetActiveInHierarchy(activeInHierarchy);
			}
		}
		OnActiveChangedEvent.Invoke(this); // Wywo�ywanie eventu
	}
}

void GameObject::UpdateActiveInChildren()
{
	for (int index = 0; index < _transform->GetChildCount(); ++index)
	{
		_transform->GetChildAt(index)->GetGameObject()->SetActiveInHierarchy(_activeSelf);
	}
}

GameObject::GameObject(size_t id) {
	
	// Setting IDs
	_id = id;
	if (_currentFreeId <= id) {
		for (; _currentFreeId < id; ++_currentFreeId) _freedIds.push_back(_currentFreeId);
		sort(_freedIds.begin(), _freedIds.end());
		_currentFreeId = id + 1;
	}
	else {
		if (_freedIds.size() > 0) {
			for (size_t i = 0; i < _freedIds.size(); ++i) {
				if (_freedIds[i] == _id) {
					_freedIds.erase(_freedIds.begin() + i);
					break;
				}
			}
		}
	}

	// Setting activation
	_activeInHierarchy = true;
	_activeSelf = true;

	// Setting is no static by default
	_isStatic = false;

	// Setting default name
	_name = "New GameObject";

	_transform = new Transform();

	//components = list<Component*>();
	components = std::list<Component*>();
	components.push_back(_transform);
}

GameObject::GameObject()
{
	// Setting ID
	_id = GetFreeId();

	// Setting activation
	_activeInHierarchy = true;
	_activeSelf = true;

	// Setting is no static by default
	_isStatic = false;

	// Setting default name
	_name = "New GameObject";

	_transform = new Transform();
	_transform->Init(this);

	//components = list<Component*>();
	components = std::list<Component*>();
	components.push_back(_transform);
}

GameObject::~GameObject()
{
	OnDestroyedEvent(this);

	for (Component* component : components)
	{
		component->OnDestroy();
	}

	for (Component* component : components)
	{
		delete component;
	}

	components.clear();
	FreeId(_id);
}

size_t GameObject::Id() const
{
	return _id;
}

bool GameObject::GetActiveInHierarchy() const
{
	return _activeInHierarchy;
}

bool GameObject::GetActiveSelf() const
{
	return _activeSelf;
}

bool GameObject::GetActive() const
{
	return _activeSelf && _activeInHierarchy;
}

void GameObject::SetActive(bool active)
{
	if (_activeSelf != active)
	{
		_activeSelf = active;

		//SetActiveInHierarchy(active);
		for (int index = 0; index < _transform->GetChildCount(); ++index)
		{
			_transform->GetChildAt(index)->GetGameObject()->SetActiveInHierarchy(_activeSelf);
		}

		OnActiveChangedEvent.Invoke(this); // Wywo�ywanie eventu
	}
}

bool GameObject::GetIsStatic() const
{
	return _isStatic;
}

void GameObject::SetIsStatic(bool isStatic)
{
	if (_isStatic != isStatic)
	{
		_isStatic = isStatic;
		OnStaticChangedEvent.Invoke(this);
	}
}

Transform* GameObject::GetTransform() const
{
	return _transform;
}

string GameObject::GetName() const
{
	return _name;
}

void GameObject::SetName(const string& name)
{
	_name = name;
}

void GameObject::Update()
{
	UpdateComponents();

	for (size_t i = 0; i < _transform->GetChildCount(); ++i)
	{
		GameObject* child = _transform->GetChildAt(i)->GetGameObject();
		if (child->GetActive()) child->Update();
	}
}

void GameObject::UpdateComponents()
{
	for (Component* component : components)
	{
		if (component->IsEnable()) component->Update();
	}
}

void GameObject::AddTag(std::string_view tagName) {
	auto tagItr = AllTags.find(tagName);
	if (tagItr == AllTags.end()) {
		AllTags.insert(tagName);
		tagItr = AllTags.find(tagName);
	}

	tagsIndexes.insert(std::distance(AllTags.begin(), tagItr));
}

void GameObject::RemoveTag(std::string_view tagName) {
	auto tagItr = AllTags.find(tagName);
	if (tagItr != AllTags.end()) {
		tagsIndexes.erase(std::distance(AllTags.begin(), tagItr));
	}
}

bool GameObject::HasTag(std::string_view tagName) {
	auto tagItr = AllTags.find(tagName);
	if (tagItr != AllTags.end()) {
		return tagsIndexes.contains((char)std::distance(AllTags.begin(), tagItr));
	}
	else {
		return false;
	}
}

YAML::Node GameObject::Serialize() const
{
	YAML::Node node;
	node["id"] = _id;
	node["name"] = _name;
	node["isStatic"] = _isStatic;
	node["isActive"] = _activeSelf;
	node["transform"] = _transform->Serialize();
	node["components"] = vector<YAML::Node>();
	for (const Component* comp : components) {
		if (comp != _transform)
			node["components"].push_back(comp->Serialize());
	}
	node["children"] = vector<YAML::Node>();
	for (size_t i = 0; i < _transform->GetChildCount(); ++i) {
		node["children"].push_back(_transform->GetChildAt(i)->GetGameObject()->Id());
	}
	return node;
}

bool GameObject::Deserialize(const YAML::Node& node) {
	if (!node["name"] || !node["isStatic"] || !node["isActive"]) return false;

	_name = node["name"].as<string>();
	_isStatic = node["isStatic"].as<bool>();
	
	SetActive(node["isActive"].as<bool>());

	return true;
}

#if _DEBUG
void GameObject::DrawEditor()
{
	string id = std::to_string(_id);
	string buff = _name;
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll;

	if (ImGui::InputText(string("Name##GO").append(id).c_str(), &buff, flags)) {
		if (buff != _name) {
			if (buff.size() == 0) {
				buff = "GameObject";
			}
			SetName(buff);
		}
	}

	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50);

	if (ImGui::Button(string(ICON_FA_DOWNLOAD "##Save To Prefab GO").append(id).c_str())) {
		_saveGameObjectAsPrefab = true;
		_fileDialogPrefabSaveInfo.type = ImGuiFileDialogType_SaveFile;
		_fileDialogPrefabSaveInfo.title = string("Save Prefab##File_Prefab_Save").append(id);
		_fileDialogPrefabSaveInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\prefabs");
	}

	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);

	if (ImGui::Button(string(ICON_FA_TRASH_CAN "##Remove GO").append(id).c_str())) {
		Manager::SceneManager::DestroyGameObject(this);
		return;
	}

	if (ImGui::FileDialog(&_saveGameObjectAsPrefab, &_fileDialogPrefabSaveInfo))
	{
		// Result path in: m_fileDialogInfo.resultPath
		std::string path = std::filesystem::relative(_fileDialogPrefabSaveInfo.resultPath).string();
		std::string name = std::filesystem::path(path).stem().string();
		PrefabManager::SaveAsPrefab(this, path);
	}

	ImGui::Text("Id: %d", _id);

	bool v = _isStatic;
	ImGui::Checkbox(string("Static##GO").append(id).c_str(), &v);

	if (v != _isStatic) {
		SetIsStatic(v);
	}

	bool v2 = _activeSelf;
	ImGui::Checkbox(string("Active##GO").append(id).c_str(), &v2);

	SetActive(v2);

	_transform->DrawEditor();
	ImGui::Separator();

	std::list cs(components);

	for (size_t i = 0; i < components.size(); ++i) {
		if (cs.front() != _transform) {
			cs.front()->DrawEditor();
			ImGui::Separator();
		}
		cs.pop_front();
	}

	if (ImGui::Button(string("Add Component##GO").append(id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
		ImGui::OpenPopup(string("Add Component PopUp##GO").append(id).c_str(), ImGuiPopupFlags_NoReopen);
	}

	if (ImGui::BeginPopup(string("Add Component PopUp##GO").append(id).c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {

		map<size_t, string> types = ComponentsMap::GetComponentsTypes();

		vector<string> names = vector<string>();
		names.resize(types.size());

		std::transform(types.begin(), types.end(), names.begin(), [](std::pair<size_t, string> const& i) -> string {
			return i.second;
		});

		types.clear();

		std::sort(names.begin(), names.end(), [&](string const& left, string const& right) -> bool {
			return left.compare(right) < 0;
		});

		int choosed = -1;
		bool clicked = false;

		if (ImGui::ComboWithFilter(string("##GO POP UP COMPONENTS").append(id).c_str(), &choosed, names, 20)) {
			if (choosed != -1) {
				Component* comp = ComponentsMap::CreateComponent(names[choosed]);
				this->AddComponent(comp);
			}

			clicked = true;
		}

		/*
		if (ImGui::BeginCombo(string("##GO POP UP COMPONENTS").append(id).c_str(), choosed == 0 ? "None" : types[choosed].c_str())) {

			bool clicked = false;
			for (auto& item : types) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).c_str(), item.first == choosed)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed != 0) {
					Component* comp = ComponentsMap::CreateComponent(types[choosed]);
					this->AddComponent(comp);
				}
			}

			ImGui::EndCombo();
		}
		*/

		if (choosed != -1) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
#endif

void GameObject::AddComponent(Component* comp)
{
	components.push_back(comp);
	comp->Init(this);
	comp->Initialize();
	comp->OnEnable();
}

void GameObject::AddComponentNoInit(Component* comp)
{
	components.push_back(comp);
}

void GameObject::RemoveComponent(Component* component)
{
	size_t temp = components.size();
	size_t temp2 = components.remove(component);
	if (temp2 < temp)
	{
		component->OnDestroy();
		delete component;
	}
	//std::remove_if(components.begin(), components.end(), [component](Component* comp) { return comp == component; });
}

GameObject* GameObject::Instantiate(GameObject* gameObject)
{
	return gameObject->Clone();
}

GameObject* GameObject::Instantiate(GameObject* gameObject, Transform* parent)
{
	GameObject* cloned = gameObject->Clone();

	cloned->GetTransform()->SetParent(parent);

	return cloned;
}