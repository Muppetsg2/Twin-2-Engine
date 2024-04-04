#ifndef _GAMECOLLIDER_H_
#define _GAMECOLLIDER_H_

#include "Collider.h"
#include "BoundingVolume.h"
#include "LayersData.h"
#include <Ray.h>
#include <core/ColliderComponent.h>

//#define GameColliderPtr GameCollider*

namespace Twin2Engine {
	namespace Core {
		class ColliderComponent;
	}
}

namespace CollisionSystem {
	class Ray;

	class GameCollider : public Collider {
		private:
			std::unordered_set<GameCollider*> LastFrameCollisions;
			//Collision* test(Collider* collider, bool separate) const;
	
		public:
			bool isTrigger = false;
			bool isStatic = false;
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
	
			Collision* rayCollision(Ray& ray);
	};
}

#endif // !_GAMECOLLIDER_H_
