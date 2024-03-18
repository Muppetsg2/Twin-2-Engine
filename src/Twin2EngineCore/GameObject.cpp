#include "GameObject.h"

unsigned int Twin2EngineCore::GameObject::_currentFreeId = 1;

Twin2EngineCore::GameObject::GameObject()
{
	// Setting ID
	_id = _currentFreeId++;

	// Setting activation
	_activeInHierarchy = true;
	_activeSelf = true;

	// Setting is no static by default
	_isStatic = false;

	// Setting default name
	name = "New GameObject";

	transform = new Transform();
	components.push_back(transform);
}

Twin2EngineCore::GameObject::~GameObject()
{
	for (Component* component : components)
	{
		delete component;
	}
}

unsigned int Twin2EngineCore::GameObject::Id()
{
	return _id;
}

bool Twin2EngineCore::GameObject::GetActive()
{
	return _activeSelf && _activeInHierarchy;
}
void Twin2EngineCore::GameObject::SetActive(bool active)
{
	_activeSelf = active;

	//Checking if this gameObject's parents are active to go end activate childs 
	if (_activeInHierarchy)
	{
		if (_activeSelf)
		{
			//zrobiæ przejœcie po childach w celu aktywacji _activeInHierarchy
			//zaprzestanie gdy:
			// - dojdzie siê do koñca
			// - gdy gameObject ma _activeSelf == false wtedy nale¿y aktywowaæ _activeInHierarchy i go zaprzestaæ dlasze przeszukiwanie w tym kierunku
		}
		else
		{
			//wy³¹czanie na podobnej zasadzie childów
		}
	}
}



bool Twin2EngineCore::GameObject::GetIsStatic()
{
	return _isStatic;
}
void Twin2EngineCore::GameObject::SetIsStatic(bool isStatic)
{
	_isStatic = isStatic;
}


void Twin2EngineCore::GameObject::RemoveComponent(Component* component)
{
	components.remove(component);
}