#pragma once

#include <core/Component.h>
#include <physics/GameCollider.h>
#include <physics/BoundingVolume.h>
#include <tools/EventHandler.h>
#include <physics/LayersData.h>

namespace Twin2Engine::PhysicsEngine {
	class GameCollider;
	class BoundingVolume;
}

namespace Twin2Engine::Core {
	//class GameCollider;
	class ColliderComponent;

	class ColliderComponent : public Component {
	private:
		bool dirtyFlag = false;
		size_t _onCollisionEnterId = 0;

		static void OnCollisionEnter(PhysicsEngine::Collision* collision);
	protected:
		ColliderComponent(); // Powoduje ze klasa jest jakby abstrakcyjna no chyba ze bedzie dziedziczona
		PhysicsEngine::GameCollider* collider = nullptr;
		PhysicsEngine::BoundingVolume* boundingVolume = nullptr;

	public:
		virtual ~ColliderComponent();

		void SetTrigger(bool v);
		bool IsTrigger();
		void SetStatic(bool v);
		bool IsStatic();
		void SetLayer(PhysicsEngine::Layer layer);
		PhysicsEngine::Layer GetLayer();
		void SetLayersFilter(PhysicsEngine::LayerCollisionFilter& layersFilter);

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;

		void EnableBoundingVolume(bool v);
		void SetBoundingVolumeRadius(float radius);
		void SetLocalPosition(float x, float y, float z);

		Tools::EventHandler<PhysicsEngine::Collision*>& GetOnTriggerEnterEvent() const;
		Tools::EventHandler<PhysicsEngine::GameCollider*>& GetOnTriggerExitEvent() const;
		Tools::EventHandler<PhysicsEngine::Collision*>& GetOnCollisionEnterEvent() const;
		Tools::EventHandler<PhysicsEngine::GameCollider*>& GetOnCollisionExitEvent() const;

		virtual YAML::Node Serialize() const override;
	};
}