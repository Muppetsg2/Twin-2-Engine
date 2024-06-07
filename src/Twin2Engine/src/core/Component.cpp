#include <core/Component.h>
#include <core/GameObject.h>
#include <manager/SceneManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace std;

size_t Component::_currentFreeId = 0;
list<size_t> Component::_freedIds;

size_t Component::GetFreeId()
{
	size_t id;
	if (_freedIds.size() > 0) {
		id = _freedIds.front();
		_freedIds.pop_front();
	}
	else {
		id = _currentFreeId++;
	}
	return id;
}

void Component::FreeId(size_t id)
{
	_freedIds.push_back(id);
	_freedIds.sort();
}

Component::Component()
{
	_id = 0;
	_gameObject = nullptr;
	_enabled = true;
}

#if _DEBUG
bool Component::DrawInheritedFields()
{
	string id = string(std::to_string(this->GetId()));
	bool e = _enabled;
	ImGui::Checkbox(string("Enable##").append(id).c_str(), &e);

	if (e != _enabled) {
		SetEnable(e);
	}

	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
	if (ImGui::Button(std::string(ICON_FA_TRASH_CAN "##Remove").append(id).c_str())) {
		_gameObject->RemoveComponent(this);
		return true;
	}
	return false;
}
#endif

Component::~Component()
{
	SceneManager::RemoveComponentWithId(this);
	_gameObject = nullptr;
}

void Component::Initialize() {}

void Component::Update() {}

void Component::OnEnable() {}

void Component::OnDisable() {}

void Component::OnDestroy() {}

YAML::Node Component::Serialize() const
{
	YAML::Node node;
	node["id"] = _id;
	node["enabled"] = _enabled;
	node["type"] = "Component";
	return node;
}

bool Component::Deserialize(const YAML::Node& node) {
	if (!node["enabled"]) return false;

	_enabled = node["enabled"].as<bool>();

	return true;
}

#if _DEBUG
void Component::DrawEditor() {
	string name = string("Component##Component").append(std::to_string(this->GetId()));
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;
	}
}
#endif

void Component::SetEnable(bool enable)
{
	if (_enabled == enable) return;

	_enabled = enable;
	if (_enabled) OnEnable();
	else OnDisable();
}

size_t Component::GetId() const
{
	return _id;
}

GameObject* Component::GetGameObject() const
{
	return _gameObject;
}

bool Component::IsEnable() const
{
	return _enabled;
}

Transform* Component::GetTransform() const
{
	return _gameObject->GetTransform();
}

string Component::GetName() const
{
	return _gameObject->GetName();
}

void Component::Init(GameObject* obj)
{
	_id = GetFreeId();
	SceneManager::AddComponentWithId(this);
	_gameObject = obj;
}

void Component::Init(GameObject* obj, size_t id)
{
	_id = id;
	if (_freedIds.size() > 0) {
		auto found = find_if(_freedIds.begin(), _freedIds.end(), [&](size_t fId) -> bool { return fId == _id; });
		if (found != _freedIds.end()) {
			_freedIds.erase(found);
		}
	}
	if (_currentFreeId <= _id) {
		for (; _currentFreeId < _id; ++_currentFreeId) _freedIds.push_back(_currentFreeId);
		_freedIds.sort();
		_currentFreeId = _id + 1;
	}
	_gameObject = obj;
}