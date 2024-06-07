#pragma once

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class SphereColliderComponent : public ColliderComponent {
	private:
		Tools::Action<Transform*> TransformChangeAction;
		size_t TransformChangeActionId = 0;

	protected:
		virtual void UnDirty() override;

	public:
		void SetRadius(float radius);

		void Initialize() override;
		void Update() override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDestroy() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}