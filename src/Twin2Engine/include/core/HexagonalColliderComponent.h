#pragma once

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class HexagonalColliderComponent : public ColliderComponent {
	private:
		Tools::Action<Transform*> TransformChangeAction;
		size_t TransformChangeActionId = 0;

	protected:
		virtual void UnDirty() override;

	public:
		void SetBaseLength(float v);
		//Along Y
		void SetHalfHeight(float v);

		void SetYRotation(float v);

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