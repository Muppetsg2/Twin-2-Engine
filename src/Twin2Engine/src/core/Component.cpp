#include <core/Component.h>
#include <core/GameObject.h>

using namespace Twin2Engine::Core;
using namespace std;

static size_t GetUniqueComponentId() {
	static size_t id = 0;
	return id++;
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
	_id = GetUniqueComponentId();
	_gameObject = obj;
	_enabled = true;
	Initialize();
	OnEnable();
}