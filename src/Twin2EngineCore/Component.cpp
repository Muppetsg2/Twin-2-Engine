#include <Component.h>

using namespace Twin2EngineCore;

static size_t GetNewComponentId() {
	static size_t id = 0;
	return id++;
}

Component::Component()
{
	_id = GetNewComponentId();
	_gameObject = nullptr;
	_enabled = true;
	Inizialize();
}

void Component::Inizialize()
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

void Component::setGameObject(GameObject* obj)
{
	_gameObject = obj;
}

void Component::setEnable(bool enable)
{
	if (_enabled == enable) return;

	_enabled = enable;
	if (_enabled) OnEnable();
	else OnDisable();
}

size_t Component::getId() const
{
	return _id;
}

GameObject* Component::getGameObject() const
{
	return _gameObject;
}

bool Component::isEnable() const
{
	return _enabled;
}