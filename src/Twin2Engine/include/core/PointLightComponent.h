#pragma once

#include <graphic/Light.h>
#include <core/LightComponent.h>

namespace Twin2Engine::Core {
	class PointLightComponent : public LightComponent {
		friend class GameObject;
	private:
		bool dirtyFlag = false;
		Graphic::PointLight* light = new Graphic::PointLight();
		Tools::Action<Transform*> OnChangePosition;
		size_t OnChangePositionId;

	public:
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;

		void SetColor(glm::vec3 color);
		void SetPower(float power);
		void SetAttenuation(float constant, float linear, float quadratic);

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}