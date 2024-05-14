#pragma once

#include <core/LightComponent.h>

namespace Twin2Engine {
	namespace Core {
		class PointLightComponent : public LightComponent {
			friend class GameObject;

			private:
				bool dirtyFlag = false;
				LightingSystem::PointLight* light = new LightingSystem::PointLight;
				Twin2Engine::Core::Action<Transform*> OnChangePosition;
				size_t OnChangePositionId;
			//protected:
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
				virtual void DrawEditor() override;
		};
	}
}