#ifndef _COLLISIONMANAGER_H_
#define _COLLISIONMANAGER_H_

//#define USE_BOUNDING_VOLUMES

#include "GameCollider.h"
#include "BoundingVolume.h"

namespace CollisionSystem {
	class CollisionManager {
		private:
			static CollisionManager* instance;
			CollisionManager();

		public:
	
	
			std::unordered_set<GameCollider*> DefaultLayer;
			std::unordered_set<GameCollider*> IgnoreRaycastLayer;
			std::unordered_set<GameCollider*> IgnoreCollisionLayer;
			std::unordered_set<GameCollider*> UiLayer;
			std::unordered_set<GameCollider*> Layer1;
			std::unordered_set<GameCollider*> Layer2;
			std::unordered_set<GameCollider*> Layer3;
			std::unordered_set<GameCollider*> Layer4;
	
	#ifdef USE_BOUNDING_VOLUMES
	
	#endif // USE_BOUNDING_VOLUMES
	
	
			static CollisionManager* Instance();

			void RegisterCollider(GameCollider* collider);
			void UnregisterCollider(GameCollider* collider);
			void PerformCollisions();
			bool Raycast(Ray& ray, RaycastHit& raycastHit);
	};
}



#endif // !_COLLISIONMANAGER_H_
