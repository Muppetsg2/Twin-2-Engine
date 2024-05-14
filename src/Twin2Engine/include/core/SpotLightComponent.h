#pragma once

#include <core/LightComponent.h>

namespace Twin2Engine {
	namespace Core {
		class SpotLightComponent : public LightComponent {
			friend class GameObject;

			private:
				bool dirtyFlag = false;
				glm::vec3 localDirection;
				LightingSystem::SpotLight* light = new LightingSystem::SpotLight;
				Twin2Engine::Core::Action<Transform*> OnChangeTransform;
				size_t OnChangeTransformId;
			//protected:
			public:
				virtual void Initialize() override;
				virtual void Update() override;
				virtual void OnEnable() override;
				virtual void OnDisable() override;
				virtual void OnDestroy() override;

				void SetDirection(glm::vec3 dir);
				void SetColor(glm::vec3 color);
				void SetPower(float power);
				void SetInnerCutOff(float radAngle);
				void SetOuterCutOff(float radAngle);
				void SetAttenuation(float constant, float linear, float quadratic);

				virtual YAML::Node Serialize() const override;
				virtual void DrawEditor() override;
		};
	}
}