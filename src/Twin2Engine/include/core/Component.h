#pragma once

namespace Twin2Engine::Manager {
	class SceneManager;
}

namespace Twin2Engine::Core
{
	class GameObject;
	class Transform;

	class Component
	{
	private:
		static size_t _currentFreeId;
		static std::list<size_t> _freedIds;
		static size_t GetFreeId();
		static void FreeId(size_t id);

		size_t _id;
		GameObject* _gameObject;
		bool _enabled;

	protected:
		Component();
	public:
		virtual ~Component();

#pragma region VirtualMethods
		virtual void Initialize();
		virtual void Update();
		virtual void OnEnable();
		virtual void OnDisable();
		virtual void OnDestroy();
		virtual YAML::Node Serialize() const;
		virtual bool Deserialize(const YAML::Node& node);
		virtual void DrawEditor();
#pragma endregion

#pragma region Setters
		void SetEnable(bool enable);
#pragma endregion

#pragma region Getters
		size_t GetId() const;
		GameObject* GetGameObject() const;
		bool IsEnable() const;

		Transform* GetTransform() const;
		std::string GetName() const;
		//GetTag() const;
		//GetLayer() const;
#pragma endregion

#pragma region FriendMethods
	private:
		void Init(GameObject* obj);
		void Init(GameObject* obj, size_t id);
	public:
		friend class GameObject;
		friend class Manager::SceneManager;
#pragma endregion

#pragma region CLONING_COMPONENTS
	protected:
		virtual Component* Clone() const
		{
			Component* clonedComonent = new Component();

			CloneTo(clonedComonent);

			return clonedComonent;
		}
		void CloneTo(Component* clonedComponent) const
		{
			clonedComponent->_enabled = _enabled;
		} 

#pragma endregion
	};
}