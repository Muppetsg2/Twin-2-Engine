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

	protected:
		Component(); // Powoduje ¿e klasa jest jakby abstrakcyjna no chyba ¿e bêdzie dziedziczona
	public:
		virtual ~Component() = default;

#pragma region VirtualMethods
		virtual void Inizialize();
		virtual void Update();
		virtual void OnEnable();
		virtual void OnDisable();
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