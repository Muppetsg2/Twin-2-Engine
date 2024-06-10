#include <core/Component.h>
#include <core/GameObject.h>
#include <manager/SceneManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace std;

size_t Component::_currentFreeId = 0;
vector<size_t> Component::_freedIds;

size_t Component::GetFreeId()
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

void Component::FreeId(size_t id)
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

	_gameObject = obj;
}