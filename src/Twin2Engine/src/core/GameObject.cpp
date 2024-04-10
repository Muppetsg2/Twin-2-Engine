#include <core/GameObject.h>
#include <core/Scene.h>

using namespace Twin2Engine::Core;

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

GameObject* GameObject::Instatiate(GameObject* gameObject)
{
	return gameObject->Clone();
}

GameObject* GameObject::Instatiate(GameObject* gameObject, Transform* parent)
{
	GameObject* cloned = gameObject->Clone();

	cloned->GetTransform()->SetParent(parent);

	return cloned;
}

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

	// Pomijanie Transforma z listy komponentów
	auto component = std::next(components.begin());

	for (; component != components.end(); ++component) 
	{
		Component* clonedComponent = (*component)->Clone();
		clonedComponent->Init(cloned);
		cloned->components.push_back(clonedComponent);
	}

	for (int index = 0; index < _transform->GetChildCount(); index++)
	{
		cloned->_transform->AddChild(Instatiate(_transform->GetChildAt(index)->GetGameObject(), cloned->_transform)->GetTransform());
	}
}

unsigned int Twin2Engine::Core::GameObject::Id() const
{
	return _id;
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
	if (_activeInHierarchy != activeInHierarchy) // warunek sprawdzajï¿½cy czy to ustawienie zmieni stan (musi zmieniaï¿½ inaczej nie ma sensu dziaï¿½ï¿½ï¿½ dalej)
	{
		_activeInHierarchy = activeInHierarchy; //zmiana stanu
		if (_activeSelf) // sprawdzenie wï¿½asnego stanu, jeï¿½li ustawiony na false to znaczy, ï¿½e ten stan dyktuje warunki aktywnoï¿½ci wszystkich podrzï¿½dnych obiektï¿½w
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
		GameObject* child = _transform->GetChildAt(i)->GetGameObject();
		if (child->GetActive()) child->Update();
	}
}

void Twin2Engine::Core::GameObject::UpdateComponents()
{
	for (Component* component : components)
	{
		if (component->IsEnable()) component->Update();
	}
}


void Twin2Engine::Core::GameObject::RemoveComponent(Component* component)
{
	components.remove(component);
	//std::remove_if(components.begin(), components.end(), [component](Component* comp) { return comp == component; });
}