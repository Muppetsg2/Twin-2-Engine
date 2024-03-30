#pragma once

namespace Twin2Engine
{
	namespace Core {
		class GameObject;
		class Transform;

		class Component
		{
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
			void setEnable(bool enable);
#pragma endregion

#pragma region Getters
			size_t getId() const;
			GameObject* getGameObject() const;
			bool isEnable() const;

			Transform* getTransform() const;
			std::string getName() const;
			//getTag() const;
			//getLayer() const;
#pragma endregion

#pragma region FriendMethods
		private:
			void Init(GameObject* obj);
		public:
			friend GameObject;
#pragma endregion
		};
	}
}