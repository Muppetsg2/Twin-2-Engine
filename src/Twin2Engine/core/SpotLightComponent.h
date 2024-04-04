#ifndef _SPOTLIGHTCOMPONENT_H_
#define _SPOTLIGHTCOMPONENT_H_

#include <core/LightComponent.h>

namespace Twin2Engine {
	namespace Core {
		class SpotLightComponent : public LightComponent {
			friend class GameObject;

			private:
				bool dirtyFlag = false;
				glm::vec3 localDirection;
				LightingSystem::SpotLight* light = nullptr;
				Twin2Engine::Core::Action<> OnChangeTransform;
			protected:
				SpotLightComponent() : LightComponent() {};
			public:

				virtual void Initialize() override;
				virtual void Update() override;
				virtual void OnEnable() override;
				virtual void OnDisable() override;
				virtual void OnDestroy() override;

				void SetDirection(glm::vec3& dir);
				void SetColor(glm::vec3& color);
				void SetPower(float& power);
				void SetInnerCutOff(float radAngle);
				void SetOuterCutOff(float radAngle);
				void SetAtenuation(float constant, float linear, float quadratic);
		};
	}
}

#endif // !_SPOTLIGHTCOMPONENT_H_
