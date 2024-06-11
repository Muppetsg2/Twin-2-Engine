#pragma once

using std::list;
using std::string;

#include <core/Component.h>
#include <core/Transform.h>
#include <physic/LayersData.h>

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
		static std::vector<size_t> _freedIds;
		static size_t GetFreeId();
		static void FreeId(size_t id);
		
		static std::unordered_set<std::string_view> AllTags;
		std::unordered_set<char> tagsIndexes;

		size_t _id;
		string _name;

		Transform* _transform;
		std::list<Component*> components;

		bool _activeSelf;
		bool _activeInHierarchy;

		bool _isStatic;

#if _DEBUG
		// For ImGui
		ImFileDialogInfo _fileDialogPrefabSaveInfo;
		bool _saveGameObjectAsPrefab = false;
#endif

		//Layer
		//Tag
		void SetActiveInHierarchy(bool activeInHierarchy);
		void UpdateActiveInChildren();

		GameObject(size_t id);
	public:
		GameObject();

#pragma region EVENTS
		Tools::EventHandler<GameObject*> OnActiveChangedEvent;
		Tools::EventHandler<GameObject*> OnStaticChangedEvent;
		Tools::EventHandler<GameObject*> OnDestroyedEvent;
#pragma endregion

		virtual ~GameObject();

		size_t Id() const;

		bool GetActiveInHierarchy() const;
		bool GetActiveSelf() const;
		bool GetActive() const;
		void SetActive(bool active);

		bool GetIsStatic() const;
		void SetIsStatic(bool isStatic);

		Transform* GetTransform() const;

		string GetName() const;
		void SetName(const string& name);

		void Update();
		void UpdateComponents();

		void AddTag(std::string_view tagName);
		void RemoveTag(std::string_view tagName);
		bool HasTag(std::string_view tagName);
		//Layer layer = Layer::DEFAULT;

		YAML::Node Serialize() const;
		bool Deserialize(const YAML::Node& node);
		
#if _DEBUG
		void DrawEditor();
#endif

#pragma region COMPONENTS_MANAGEMENT

		template<class T>
		T* AddComponent();

	private:
		void AddComponent(Component* comp);
		void AddComponentNoInit(Component* comp);
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
		template<class T>
		typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
			GetComponentInParents();
		template<class T>
		list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
			GetComponentsInParents();

		void RemoveComponent(Component* component);
		template<class T, std::enable_if<std::is_base_of<Component, T>::value, bool>::type = true>
		void RemoveComponents() {
			components.remove_if([](Component* component) { 
				if (dynamic_cast<T*>(component) != nullptr)
				{
					component->OnDestroy();
					//delete component;
					return true;
				}
				return false;
			});
		}

#pragma endregion
	
		static GameObject* Instantiate(GameObject* gameObject);
		static GameObject* Instantiate(GameObject* gameObject, Transform* parent);
	};
}


template<class T>
T* Twin2Engine::Core::GameObject::AddComponent()
{
	static_assert(std::is_base_of<Component, T>::value);
	T* component = new T();

	AddComponent(component);

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
	//std::copy_if(components.begin(), components.end(), std::back_inserter(foundComponents), [](Component* component) { return dynamic_cast<T*>(component) != nullptr; });
	for (auto itr = components.begin(); itr != components.end(); itr++)
	{
		T* componentCast = dynamic_cast<T*>(*itr);
		if (componentCast != nullptr)
		{
			foundComponents.push_back(componentCast);
		}
	}
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
	list<T*> comps;
	for (size_t i = 0; i < _transform->GetChildCount(); ++i) {
		GameObject* obj = _transform->GetChildAt(i)->GetGameObject();
		comps.merge(obj->GetComponents<T>());
		comps.merge(obj->GetComponentsInChildren<T>());
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
	if (parent == nullptr) return list<T*>();
	return parent->GetGameObject()->GetComponents<T>();
}

template<class T>
typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
Twin2Engine::Core::GameObject::GetComponentInParents()
{
	Transform* parent = _transform->GetParent();
	if (parent == nullptr) return nullptr;
	T* comp = parent->GetGameObject()->GetComponent<T>();
	if (comp == nullptr) return parent->GetGameObject()->GetComponentInParents<T>();
	return comp;
}

template<class T>
list<typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type>
Twin2Engine::Core::GameObject::GetComponentsInParents()
{
	Transform* parent = _transform->GetParent();
	if (parent == nullptr) return list<T*>();

	list<T*> comps;
	comps.merge(parent->GetGameObject()->GetComponents<T>());
	comps.merge(parent->GetGameObject()->GetComponentsInParents<T>());
	return comps;
}