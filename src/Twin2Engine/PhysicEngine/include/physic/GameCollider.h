#ifndef _GAMECOLLIDER_H_
#define _GAMECOLLIDER_H_

#include <physic/Collider.h>
#include <physic/BoundingVolume.h>
#include <physic/LayersData.h>
#include <physic/Ray.h>
#include <graphic/Frustum.h>
#include <tools/EventHandler.h>

namespace Twin2Engine::Core {
	class ColliderComponent;
}

namespace Twin2Engine::Physic {
	class Ray;
	struct RaycastHit;

	class GameCollider : public Collider {
	private:
		std::unordered_set<GameCollider*> LastFrameCollisions;

	public:
		unsigned int colliderId = 0;

		bool enabled = false;
		bool isTrigger = false;
		bool isStatic = false;

		Tools::EventHandler<Collision*> OnTriggerEnter;
		Tools::EventHandler<GameCollider*> OnTriggerExit;
		Tools::EventHandler<Collision*> OnCollisionEnter;
		Tools::EventHandler<GameCollider*> OnCollisionExit;

		Layer layer = Layer::DEFAULT;
		LayerCollisionFilter& layersFilter = GLOBAL_LAYERS_FILTERS[int(layer)];
		BoundingVolume* boundingVolume = nullptr;
		Twin2Engine::Core::ColliderComponent* colliderComponent = nullptr;

		GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, SphereColliderData* sphereColliderData);
		GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, BoxColliderData* boxColliderData);
		GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, CapsuleColliderData* CapsuleColliderData);

		virtual ~GameCollider();

		Collision* testBoundingVolume(BoundingVolume* other) const;// = 0;
		Collision* testBoundingVolume(GameCollider* other) const;// = 0;
		Collision* testColliders(GameCollider* collider) const;// = 0;

		Collision* collide(Collider* other) override;

		bool rayCollision(Ray& ray, RaycastHit& raycastHit);
	};
}

#endif // !_GAMECOLLIDER_H_
