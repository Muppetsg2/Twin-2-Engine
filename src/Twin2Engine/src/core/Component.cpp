#include <core/Component.h>
#include <core/GameObject.h>

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
	OnDestroy();
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