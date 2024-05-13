#include <core/Component.h>
#include <core/GameObject.h>
#include <core/YamlConverters.h>

using namespace Twin2Engine::Core;
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

Component::~Component()
{
	_gameObject = nullptr;
}

void Component::Initialize()
{
}

void Component::Update()
{
}

void Component::OnEnable()
{
}

void Component::OnDisable()
{
}

void Component::OnDestroy()
{
}

YAML::Node Component::Serialize() const
{
	YAML::Node node;
	node["id"] = _id;
	node["enabled"] = _enabled;
	node["type"] = "";
	node["subTypes"] = vector<string>();
	return node;
}

void Twin2Engine::Core::Component::DrawEditor() {}

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
	_gameObject = obj;
	_enabled = true;
	Initialize();
	OnEnable();
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
	_enabled = true;
	Initialize();
	OnEnable();
}