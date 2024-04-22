#include <core/GameObject.h>
#include <core/Scene.h>

using namespace Twin2Engine::Core;

size_t Twin2Engine::Core::GameObject::_currentFreeId = 1;
list<size_t> Twin2Engine::Core::GameObject::_freedIds;

GameObject::GameObject(size_t id) {
	
	// Setting IDs
	_id = id;
	if (_freedIds.size() > 0) {
		auto found = find_if(_freedIds.begin(), _freedIds.end(), [&](size_t fId) -> bool { return fId == _id; });
		if (found != _freedIds.end()) {
			_freedIds.erase(found);
		}
	}
	if (_currentFreeId <= id) {
		for (; _currentFreeId < id; ++_currentFreeId) _freedIds.push_back(_currentFreeId);
		_freedIds.sort();
		_currentFreeId = id + 1;
	}

	// Setting activation
	_activeInHierarchy = true;
	_activeSelf = true;

	// Setting is no static by default
	_isStatic = false;

	// Setting default name
	_name = "New GameObject";

	_transform = new Transform();

	//components = list<Component*>();
	components = std::list<Component*>();
	components.push_back(_transform);
}

GameObject::GameObject()
{
	// Setting ID
	_id = GetFreeId();

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

GameObject::~GameObject()
{
	for (Component* component : components)
	{
		component->OnDestroy();
	}

	for (Component* component : components)
	{
		delete component;
	}

	components.clear();
	_freedIds.push_back(_id);
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

	// Pomijanie Transforma z listy komponent�w
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

size_t GameObject::GetFreeId()
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

void GameObject::FreeId(size_t id)
{
	_freedIds.push_back(id);
	_freedIds.sort();
}

size_t GameObject::Id() const
{
	return _id;
}

bool GameObject::GetActiveInHierarchy() const
{
	return _activeInHierarchy;
}

bool GameObject::GetActive() const
{
	return _activeSelf && _activeInHierarchy;
}

void GameObject::SetActive(bool active)
{
	_activeSelf = active;

	SetActiveInHierarchy(active);
}

void GameObject::SetActiveInHierarchy(bool activeInHierarchy)
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

bool GameObject::GetIsStatic() const
{
	return _isStatic;
}

void GameObject::SetIsStatic(bool isStatic)
{
	_isStatic = isStatic;
}

Transform* GameObject::GetTransform() const
{
	return _transform;
}

string GameObject::GetName() const
{
	return _name;
}

void GameObject::SetName(const string& name)
{
	_name = name;
}

void GameObject::Update()
{
	UpdateComponents();

	for (size_t i = 0; i < _transform->GetChildCount(); i++)
	{
		GameObject* child = _transform->GetChildAt(i)->GetGameObject();
		if (child->GetActive()) child->Update();
	}
}

void GameObject::UpdateComponents()
{
	for (Component* component : components)
	{
		if (component->IsEnable()) component->Update();
	}
}

YAML::Node GameObject::Serialize() const
{
	YAML::Node node;
	node["id"] = _id;
	node["name"] = _name;
	node["isStatic"] = _isStatic;
	node["isActive"] = _activeSelf;
	node["transform"] = _transform->Serialize();
	node["components"] = vector<YAML::Node>();
	for (const Component* comp : components) {
		if (comp != _transform)
			node["components"].push_back(comp->Serialize());
	}
	node["children"] = vector<YAML::Node>();
	for (size_t i = 0; i < _transform->GetChildCount(); ++i) {
		node["children"].push_back(_transform->GetChildAt(i)->GetGameObject()->Id());
	}
	return node;
}

void GameObject::AddComponent(Component* comp)
{
	components.push_back(comp);
}

void GameObject::RemoveComponent(Component* component)
{
	components.remove(component);
	//std::remove_if(components.begin(), components.end(), [component](Component* comp) { return comp == component; });
}