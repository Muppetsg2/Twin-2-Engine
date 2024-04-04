#ifndef _COLLIDERCOMPONENT_H_
#define _COLLIDERCOMPONENT_H_

//#include "Component.h"
#include <core/Component.h>
#include <vector>
#include <GameCollider.h>
#include <BoundingVolume.h>
#include <core/EventHandler.h>
#include <LayersData.h>

namespace CollisionSystem {
	class GameCollider;
	class BoundingVolume;
}

namespace Twin2Engine::Core {
	//class GameCollider;
	class ColliderComponent;
	//std::vector<ColliderComponent*> colliderComponents = std::vector<ColliderComponent*>();

	struct Collision {
		ColliderComponent* collider;
		ColliderComponent* otherCollider;
		glm::vec3 position;
	};

	class ColliderComponent : public Component {
	protected:
		bool dirtyFlag = false;
		ColliderComponent(); // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
		CollisionSystem::GameCollider * collider = nullptr;
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
		virtual void Initialize() override;
		virtual void Update() override;
	};
}

#endif // !_COLLIDERCOMPONENT_H_



/*/#pragma once

#include "Component.h"
#include <vector>

namespace Twin2Engine {
	namespace Core {
		class ColliderComponent;
		static std::vector<ColliderComponent*> colliderComponents = std::vector<ColliderComponent*>();

		class ColliderComponent : public Component {
		protected:
			ColliderComponent(); // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
		public:
			virtual ~ColliderComponent();
		};
	}
}/**/