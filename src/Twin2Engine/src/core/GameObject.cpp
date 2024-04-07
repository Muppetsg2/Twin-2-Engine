#include <core/GameObject.h>
#include <core/Scene.h>

unsigned int Twin2Engine::Core::GameObject::_currentFreeId = 1;

Twin2Engine::Core::GameObject::GameObject(Scene* scene)
{
	_scene = scene;

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
	((Component*)_transform)->Init(this);

	//components = list<Component*>();
	components = std::list<Component*>();
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

Twin2Engine::Core::Scene* Twin2Engine::Core::GameObject::GetObjectScene() const
{
	return _scene;
}

bool Twin2Engine::Core::GameObject::GetActiveInHierarchy() const
{
	return _activeInHierarchy;
}

bool Twin2Engine::Core::GameObject::GetActive() const
{
	return _activeSelf && _activeInHierarchy;
}
void Twin2Engine::Core::GameObject::SetActive(bool active)
{
	_activeSelf = active;

	SetActiveInHierarchy(active);
}

void Twin2Engine::Core::GameObject::SetActiveInHierarchy(bool activeInHierarchy)
{
	if (_activeInHierarchy != activeInHierarchy) // warunek sprawdzaj�cy czy to ustawienie zmieni stan (musi zmienia� inaczej nie ma sensu dzia��� dalej)
	{
		_activeInHierarchy = activeInHierarchy; //zmiana stanu
		if (_activeSelf) // sprawdzenie w�asnego stanu, je�li ustawiony na false to znaczy, �e ten stan dyktuje warunki aktywno�ci wszystkich podrz�dnych obiekt�w
		{
			for (int index = 0; index < _transform->GetChildCount(); index++)
			{
				_transform->GetChildAt(index)->GetGameObject()->SetActiveInHierarchy(activeInHierarchy);
			}
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

void Twin2Engine::Core::GameObject::Update()
{
	UpdateComponents();

	for (size_t i = 0; i < _transform->GetChildCount(); i++)
	{
		_transform->GetChildAt(i)->GetGameObject()->Update();
	}
}

void Twin2Engine::Core::GameObject::UpdateComponents()
{
	for (Component* component : components)
	{
		component->Update();
	}
}


void Twin2Engine::Core::GameObject::RemoveComponent(Component* component)
{
	components.remove(component);
	//std::remove_if(components.begin(), components.end(), [component](Component* comp) { return comp == component; });
}