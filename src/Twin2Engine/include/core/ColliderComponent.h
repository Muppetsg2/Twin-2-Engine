#pragma once

#include <core/Component.h>
#include <physic/GameCollider.h>
#include <physic/BoundingVolume.h>
#include <tools/EventHandler.h>
#include <physic/LayersData.h>

namespace Twin2Engine::Physic {
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

		static void OnCollisionEnter(Physic::Collision* collision);
	protected:
		ColliderComponent(); // Powoduje ze klasa jest jakby abstrakcyjna no chyba ze bedzie dziedziczona
		Physic::GameCollider* collider = nullptr;
		Physic::BoundingVolume* boundingVolume = nullptr;

	public:
		virtual ~ColliderComponent();

		void SetTrigger(bool v);
		bool IsTrigger();
		void SetStatic(bool v);
		bool IsStatic();
		void SetLayer(Physic::Layer layer);
		Physic::Layer GetLayer();
		void SetLayersFilter(Physic::LayerCollisionFilter& layersFilter);

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;

		void EnableBoundingVolume(bool v);
		void SetBoundingVolumeRadius(float radius);
		void SetLocalPosition(float x, float y, float z);

		Tools::EventHandler<Physic::Collision*>& GetOnTriggerEnterEvent() const;
		Tools::EventHandler<Physic::GameCollider*>& GetOnTriggerExitEvent() const;
		Tools::EventHandler<Physic::Collision*>& GetOnCollisionEnterEvent() const;
		Tools::EventHandler<Physic::GameCollider*>& GetOnCollisionExitEvent() const;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
	};
}