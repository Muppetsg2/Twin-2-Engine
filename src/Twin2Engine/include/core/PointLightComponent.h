#ifndef _POINTLIGHTCOMPONENT_H_
#define _POINTLIGHTCOMPONENT_H_

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
				PointLightComponent() : LightComponent() {};

				virtual void Initialize() override;
				virtual void Update() override;
				virtual void OnEnable() override;
				virtual void OnDisable() override;
				virtual void OnDestroy() override;

				void SetColor(glm::vec3 color);
				void SetPower(float power);
				void SetAtenuation(float constant, float linear, float quadratic);

				virtual YAML::Node Serialize() const override;
		};
	}
}

#endif // !_POINTLIGHTCOMPONENT_H_
