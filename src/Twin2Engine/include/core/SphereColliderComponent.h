#pragma once

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class SphereColliderComponent : public ColliderComponent {
		//protected:
	private:
		Twin2Engine::Core::Action<Transform*> PositionChangeAction;
		size_t PositionChangeActionId = 0;
		bool dirtyFlag = false;
	public:
		SphereColliderComponent();
		void SetRadius(float radius);

		void Initialize() override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDestroy() override;

		virtual YAML::Node Serialize() const override;
		virtual void DrawEditor() override;
	};
}