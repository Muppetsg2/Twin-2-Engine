#pragma once

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class CapsuleColliderComponent : public ColliderComponent {
		//protected: 
	private:
		bool dirtyFlag = false;
		Tools::Action<Transform*> TransformChangeAction;
		size_t TransformChangeActionId = 0;
	public:
		CapsuleColliderComponent();
		void SetEndPosition(float x, float y, float z);
		void SetEndPosition(const glm::vec3& pos);
		void SetRadius(float radius);

		void Initialize() override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDestroy() override;
		void Update() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}