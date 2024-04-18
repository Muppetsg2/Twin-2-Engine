#pragma once

#include <core/Component.h>
#include <GameCollider.h>
#include <BoundingVolume.h>
#include <tools/EventHandler.h>
#include <LayersData.h>

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
		glm::vec3 separation;
	};

	class ColliderComponent : public Component {
	private:
		bool dirtyFlag = false;
	protected:
		ColliderComponent(); // Powoduje ze klasa jest jakby abstrakcyjna no chyba ze bedzie dziedziczona
		CollisionSystem::GameCollider* collider = nullptr;
		CollisionSystem::BoundingVolume* boundingVolume = nullptr;

	public:
		virtual ~ColliderComponent();

		Tools::EventHandler<Collision*> OnTriggerEnter;
		Tools::EventHandler<ColliderComponent*> OnTriggerExit;
		Tools::EventHandler<Collision*> OnCollisionEnter;
		Tools::EventHandler<ColliderComponent*> OnCollisionExit;

		unsigned int colliderId = 0;

		void SetTrigger(bool v);
		bool IsTrigger();
		void SetStatic(bool v);
		bool IsStatic();
		void SetLayer(Layer layer);
		Layer GetLayer();
		void SetLayersFilter(LayerCollisionFilter& layersFilter);

		virtual void Update() override;

		void EnableBoundingVolume(bool v);
		void SetBoundingVolumeRadius(float radius);
		void SetLocalPosition(float x, float y, float z);

		//void Invoke();
		virtual YAML::Node Serialize() const override;
	};
}