#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <algorithm>
#include <iterator>
#include <list>
#include <string>

using std::list;
using std::string;

#include "Component.h"
#include "Transform.h"

using Twin2EngineCore::Component;
//using Twin2EngineCore::Transform;


namespace Twin2EngineCore
{
	class GameObject
	{
		static unsigned int _currentFreeId;
		unsigned int _id;
		string _name;

		Transform* _transform;
		list<Component*> components;

		bool _activeSelf;
		bool _activeInHierarchy;

		bool _isStatic;

		//Layer
		//Tag

	public:
		GameObject();

		virtual ~GameObject();

		inline unsigned int Id() const;

		bool GetActive() const;
		void SetActive(bool active);

		bool GetIsStatic() const;
		void SetIsStatic(bool isStatic);

		Transform* GetTransform() const;

		string GetName() const;
		void SetName(const string& name);

		template<class T>
		typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
		AddComponent();

		template<class T>
		T* GetComponent();
		template<class T>
		list<T*> GetComponents();
		template<class T>
		T* GetComponentInChildren();
		template<class T>
		T* GetComponentsInChildren();
		template<class T>
		T* GetComponentInParent();
		template<class T>
		T* GetComponentsInParent();

		void RemoveComponent(Component* component);
		template<class T>
		void RemoveComponents();
	};
}


template<class T>
typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
Twin2EngineCore::GameObject::AddComponent()
{
	Component* component = new T();

	//component.Awake();

	components.push_back(component);

	return component;
}

template<class T>
T* Twin2EngineCore::GameObject::GetComponent()
{
	list<Component*>::iterator itr = std::find_if(components.begin(), components.end(), [](Component* component) { return dynamic_cast<T*>(component) != nullptr; });

	if (itr == components.end())
	{
		return nullptr;
	}

	return *itr;
}

template<class T>
list<T*> Twin2EngineCore::GameObject::GetComponents()
{
	list<T*> foundComponents;
	std::copy_if(components.begin(), components.end(), std::back_inserter(foundComponents), [](Component* component) { return dynamic_cast<T*>(component) != nullptr; });
	return foundComponents;
}

template<class T>
T* Twin2EngineCore::GameObject::GetComponentInChildren()
{
	return nullptr;
}

template<class T>
T* Twin2EngineCore::GameObject::GetComponentsInChildren()
{
	return nullptr;
}

template<class T>
T* Twin2EngineCore::GameObject::GetComponentInParent()
{
	return nullptr;
}

template<class T>
T* Twin2EngineCore::GameObject::GetComponentsInParent()
{
	return nullptr;
}

template<class T>
void Twin2EngineCore::GameObject::RemoveComponents()
{
	components.remove_if([](Component* component) { return dynamic_cast<T*>(component) != nullptr; });
}


#endif