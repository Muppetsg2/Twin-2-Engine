#pragma once

#include <core/LightComponent.h>

namespace Twin2Engine::Core {
	class DirectionalLightComponent : public LightComponent {
		friend class GameObject;
	private:
		bool dirtyFlag = false;
		//glm::vec3 localDirection;
		Graphic::DirectionalLight* light = new Graphic::DirectionalLight();
		Tools::Action<Transform*> OnChangePosition;
		size_t OnChangePositionId;
		Tools::Method OnViewerChange;
		size_t OnViewerChangeId;

	public:
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;

		void SetDirection(glm::vec3 dir);
		void SetColor(glm::vec3 color);
		void SetPower(float power);

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}