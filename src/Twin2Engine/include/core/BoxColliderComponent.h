#pragma once

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class BoxColliderComponent : public ColliderComponent {
		//protected:
	private:
		bool dirtyFlag = false;

		Tools::Action<Transform*> TransformChangeAction;
		size_t TransformChangeActionId = 0;

	public:
		BoxColliderComponent();
		//Along X
		void SetWidth(float v);
		//Along Z
		void SetLength(float v);
		//Along Y
		void SetHeight(float v);

		void SetXRotation(float v);
		void SetYRotation(float v);
		void SetZRotation(float v);

		void SetRotation(const glm::vec3& rot);

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