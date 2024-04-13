#ifndef _COLLIDERCOMPONENT_H_
#define _COLLIDERCOMPONENT_H_

#include <core/Component.h>
#include <GameCollider.h>
#include <BoundingVolume.h>
#include <core/EventHandler.h>

namespace CollisionSystem {
	class GameCollider;
	class BoundingVolume;
}

namespace Twin2Engine::Core {
	//class GameCollider;
	class ColliderComponent;

	struct Collision {
		ColliderComponent* collider;
		ColliderComponent* otherCollider;
		glm::vec3 position;
	};

	class ColliderComponent : public Component {
	protected:
		ColliderComponent(); // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
		CollisionSystem::GameCollider* collider = nullptr;
		CollisionSystem::BoundingVolume* boundingVolume;

	public:
		virtual ~ColliderComponent();

		EventHandler<Collision*> OnTriggerEnter;
		EventHandler<ColliderComponent*> OnTriggerExit;
		EventHandler<Collision*> OnCollisionEnter;
		EventHandler<ColliderComponent*> OnCollisionExit;

		unsigned int colliderId = 0;

		void SetTrigger(bool v);
		bool IsTrigger();
		void SetStatic(bool v);
		bool IsStatic();
		void SetLayer(Layer layer);
		Layer GetLayer();
		void SetLayersFilter(LayerCollisionFilter& layersFilter);

		void EnableBoundingVolume(bool v);
		void SetBoundingVolumeRadius(float radius);
		void SetLocalPosition(float x, float y, float z);

		void Invoke();
		void Update();
		virtual YAML::Node Serialize() const override;
	};
}

#endif // !_COLLIDERCOMPONENT_H_