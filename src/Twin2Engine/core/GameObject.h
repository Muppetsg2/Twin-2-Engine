#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <algorithm>
#include <iterator>
#include <list>
#include <string>

using std::list;
using std::string;

#include <core/Component.h>
#include <core/Transform.h>

using Twin2Engine::Core::Component;
//using Twin2EngineCore::Transform;


namespace Twin2Engine
{
	namespace Core {
		class GameObject
		{
			friend Transform;

			static unsigned int _currentFreeId;
			unsigned int _id;
			string _name;

			Transform* _transform;
			std::list<Component*> components;

			bool _activeSelf;
			bool _activeInHierarchy;

			bool _isStatic;

			//Layer
			//Tag
			void SetActiveInHierarchy(bool activeInHierarchy);

		public:
			GameObject();

			virtual ~GameObject();

			inline unsigned int Id() const;

			bool GetActiveInHierarchy() const;
			bool GetActive() const;
			void SetActive(bool active);

			bool GetIsStatic() const;
			void SetIsStatic(bool isStatic);

			Transform* GetTransform() const;

			string GetName() const;
			void SetName(const string& name);

			template<class T>
			//typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
			T* AddComponent();

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
		};
	}
}


template<class T>
//typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
T* Twin2Engine::Core::GameObject::AddComponent()
{
	T* component = new T();

	component->Init(this);

	components.push_back(component);

	return component;
}

template<class T>
typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
Twin2Engine::Core::GameObject::GetComponent()
{
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
		T* comp = _transform->GetChildAt(i)->getGameObject()->GetComponent<T>();
		if (comp != nullptr) return comp;
	}
	for (size_t i = 0; i < _transform->GetChildCount(); ++i) {
		T* comp = _transform->GetChildAt(i)->getGameObject()->GetComponentInChildren<T>();
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
		GameObject* obj = _transform->GetChildAt(i)->getGameObject();
		comps.push_back(obj->GetComponents<T>());
		comps.push_back(obj->GetComponentsInChildren<T>());
	}
	return comps;
}

template<class T>
typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
Twin2Engine::Core::GameObject::GetComponentInParent()
{
	Transform* parent = _transform->GetParent();
	if (parent == nullptr) return nullptr;
	return parent->getGameObject()->GetComponent<T>();
}

template<class T>
list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
Twin2Engine::Core::GameObject::GetComponentsInParent()
{
	Transform* parent = _transform->GetParent();
	if (parent == nullptr) return nullptr;
	return parent->getGameObject()->GetComponents<T>();
}

#endif