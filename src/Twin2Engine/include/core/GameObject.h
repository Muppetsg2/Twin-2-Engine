#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

using std::list;
using std::string;

#include <core/Component.h>
#include <core/Transform.h>

using Twin2Engine::Core::Component;

namespace Twin2Engine::Manager {
	class SceneManager;
}

namespace Twin2Engine::Core
{
	class Scene;

	class GameObject
	{
		friend class Transform;
		friend class Manager::SceneManager;
	//public:

		//CloneFunction(GameObject, _name, _activeSelf, _isStatic)

		GameObject* Clone() const;
		virtual void CloneTo(GameObject* cloned) const;

	private:

		static size_t _currentFreeId;
		static std::list<size_t> _freedIds;
		static size_t GetFreeId();
		static void FreeId(size_t id);

		size_t _id;
		string _name;

		Transform* _transform;
		std::list<Component*> components;

		bool _activeSelf;
		bool _activeInHierarchy;

		bool _isStatic;


		//Layer
		//Tag
		void SetActiveInHierarchy(bool activeInHierarchy);

		GameObject(size_t id);
	public:
		GameObject();

		virtual ~GameObject();

		size_t Id() const;

		bool GetActiveInHierarchy() const;
		bool GetActive() const;
		void SetActive(bool active);

		bool GetIsStatic() const;
		void SetIsStatic(bool isStatic);

		Transform* GetTransform() const;

		string GetName() const;
		void SetName(const string& name);

		void Update();
		void UpdateComponents();

		YAML::Node Serialize() const;

#pragma region COMPONENTS_MANAGEMENT

		template<class T>
		T* AddComponent();

	private:
		void AddComponent(Component* comp);
	public:

		template<class T>
		typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
			GetComponent();
		template<class T>
		list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
			GetComponents();
		template<class T>
		typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
			GetComponentInChildren();
		template<class T>
		list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
			GetComponentsInChildren();
		template<class T>
		typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
			GetComponentInParent();
		template<class T>
		list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
			GetComponentsInParent();

		void RemoveComponent(Component* component);
		template<class T, std::enable_if<std::is_base_of<Component, T>::value, bool>::type = true>
		void RemoveComponents() {
			components.remove_if([](Component* component) { return dynamic_cast<T*>(component) != nullptr; });
		}

#pragma endregion
	
		static GameObject* Instatiate(GameObject* gameObject);
		static GameObject* Instatiate(GameObject* gameObject, Transform* parent);
	};
}


template<class T>
T* Twin2Engine::Core::GameObject::AddComponent()
{
	static_assert(std::is_base_of<Component, T>::value);
	T* component = new T();

	component->Init(this);

	components.push_back(component);

	return component;
}

template<class T>
typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
Twin2Engine::Core::GameObject::GetComponent()
{
	if (components.size() == 0) return nullptr;

	list<Component*>::iterator itr = std::find_if(components.begin(), components.end(), [](Component* component) { return dynamic_cast<T*>(component) != nullptr; });
	//std::vector<Component*>::iterator itr = std::find_if(components.begin(), components.end(), [](Component* component) { return dynamic_cast<T*>(component) != nullptr; });

	if (itr == components.end())
	{
		return nullptr;
	}

	return static_cast<T*>(*itr);
}

template<class T>
list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
Twin2Engine::Core::GameObject::GetComponents()
{
	list<T*> foundComponents;
	std::copy_if(components.begin(), components.end(), std::back_inserter(foundComponents), [](Component* component) { return dynamic_cast<T*>(component) != nullptr; });
	return foundComponents;
}

template<class T>
typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
Twin2Engine::Core::GameObject::GetComponentInChildren()
{
	// Przeszukiwanie w szerz
	for (size_t i = 0; i < _transform->GetChildCount(); ++i) {
		T* comp = _transform->GetChildAt(i)->GetGameObject()->GetComponent<T>();
		if (comp != nullptr) return comp;
	}
	for (size_t i = 0; i < _transform->GetChildCount(); ++i) {
		T* comp = _transform->GetChildAt(i)->GetGameObject()->GetComponentInChildren<T>();
		if (comp != nullptr) return comp;
	}
	return nullptr;
}

template<class T>
list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
Twin2Engine::Core::GameObject::GetComponentsInChildren()
{
	// Przeszukiwanie w g��b
	list<T*> comps = list<T*>();
	for (size_t i = 0; i < _transform->GetChildCount(); ++i) {
		GameObject* obj = _transform->GetChildAt(i)->GetGameObject();
		auto listOfComponents = obj->GetComponents<T>();
		comps.insert(comps.cend(), listOfComponents.begin(), listOfComponents.end());
		listOfComponents = obj->GetComponentsInChildren<T>();
		comps.insert(comps.cend(), listOfComponents.begin(), listOfComponents.end());
		//comps.push_back(obj->GetComponents<T>());
		//comps.push_back(obj->GetComponentsInChildren<T>());
	}
	return comps;
}

template<class T>
typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
Twin2Engine::Core::GameObject::GetComponentInParent()
{
	Transform* parent = _transform->GetParent();
	if (parent == nullptr) return nullptr;
	return parent->GetGameObject()->GetComponent<T>();
}

template<class T>
list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
Twin2Engine::Core::GameObject::GetComponentsInParent()
{
	Transform* parent = _transform->GetParent();
	if (parent == nullptr) return nullptr;
	return parent->GetGameObject()->GetComponents<T>();
}

#endif // !_GAMEOBJECT_H_