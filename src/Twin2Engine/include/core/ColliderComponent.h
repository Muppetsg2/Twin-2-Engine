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
	class ColliderComponent;

	class ColliderComponent : public Component {
	private:
		size_t _onCollisionEnterId = 0;

		static void OnCollisionEnter(Physic::Collision* collision);

	protected:
		bool dirtyFlag = false;
#if _DEBUG
		bool DrawInheritedFields() override;
#endif
		virtual void UnDirty();

	public:
		Physic::GameCollider* collider = nullptr;
		unsigned int colliderId = 0;

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

		void SetLocalPosition(float x, float y, float z);

		Tools::EventHandler<Physic::Collision*>& GetOnTriggerEnterEvent() const;
		Tools::EventHandler<Physic::GameCollider*>& GetOnTriggerExitEvent() const;
		Tools::EventHandler<Physic::Collision*>& GetOnCollisionEnterEvent() const;
		Tools::EventHandler<Physic::GameCollider*>& GetOnCollisionExitEvent() const;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}