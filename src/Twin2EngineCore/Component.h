#pragma once

namespace Twin2EngineCore
{
	class GameObject;

	class Component
	{
	private:
		size_t _id;
		GameObject* _gameObject;
		bool _enabled;

	public:
		Component();
		virtual ~Component();

#pragma region VirtualMethods
		virtual void Inizialize();
		virtual void Update();
		virtual void OnEnable();
		virtual void OnDisable();
		virtual void OnDestroy();
#pragma endregion

#pragma region Setters
	private:
		void setGameObject(GameObject* obj);
	public:
		void setEnable(bool enable);
#pragma endregion

#pragma region Getters
		size_t getId() const;
		GameObject* getGameObject() const;
		bool isEnable() const;
#pragma endregion

		friend GameObject;
	};
}