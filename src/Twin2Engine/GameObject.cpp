#include <core/GameObject.h>

unsigned int Twin2Engine::Core::GameObject::_currentFreeId = 1;

Twin2Engine::Core::GameObject::GameObject()
{
	// Setting ID
	_id = _currentFreeId++;

	// Setting activation
	_activeInHierarchy = true;
	_activeSelf = true;

	// Setting is no static by default
	_isStatic = false;

	// Setting default name
	_name = "New GameObject";

	_transform = new Transform();
	components.push_back(_transform);
}

Twin2Engine::Core::GameObject::~GameObject()
{
	for (Component* component : components)
	{
		delete component;
	}
}

unsigned int Twin2Engine::Core::GameObject::Id() const
{
	return _id;
}

bool Twin2Engine::Core::GameObject::GetActive() const
{
	return _activeSelf && _activeInHierarchy;
}
void Twin2Engine::Core::GameObject::SetActive(bool active)
{
	_activeSelf = active;

	//Checking if this gameObject's parents are active to go end activate childs 
	if (_activeInHierarchy)
	{
		if (_activeSelf)
		{
			//zrobi� przej�cie po childach w celu aktywacji _activeInHierarchy
			//zaprzestanie gdy:
			// - dojdzie si� do ko�ca
			// - gdy gameObject ma _activeSelf == false wtedy nale�y aktywowa� _activeInHierarchy i go zaprzesta� dlasze przeszukiwanie w tym kierunku
		}
		else
		{
			//wy��czanie na podobnej zasadzie child�w
		}
	}
}



bool Twin2Engine::Core::GameObject::GetIsStatic() const
{
	return _isStatic;
}
void Twin2Engine::Core::GameObject::SetIsStatic(bool isStatic)
{
	_isStatic = isStatic;
}


Twin2Engine::Core::Transform* Twin2Engine::Core::GameObject::GetTransform() const
{
	return _transform;
}

string Twin2Engine::Core::GameObject::GetName() const
{
	return _name;
}


void Twin2Engine::Core::GameObject::SetName(const string& name)
{
	_name = name;
}


void Twin2Engine::Core::GameObject::RemoveComponent(Component* component)
{
	components.remove(component);
}