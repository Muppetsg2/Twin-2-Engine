#pragma once

#include <graphic/Light.h>
#include <core/LightComponent.h>

namespace Twin2Engine::Core {
	class SpotLightComponent : public LightComponent {
		friend class GameObject;
		private:
			bool dirtyFlag = false;
			glm::vec3 localDirection;
			Graphic::SpotLight* light = new Graphic::SpotLight();
			Tools::Action<Transform*> OnChangeTransform;
			size_t OnChangeTransformId;
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
			virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
			virtual void DrawEditor() override;
#endif
	};
}