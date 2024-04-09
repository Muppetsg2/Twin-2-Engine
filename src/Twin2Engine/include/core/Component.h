#pragma once

namespace Twin2Engine::Core
{
	class GameObject;
	class Transform;

	class Component
	{
		friend class GameObject;

	private:
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
	public:
		friend GameObject;
#pragma endregion

#pragma region CLONING_COMPONENTS
	protected:
		Component* Clone() const
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