#ifndef _DIRECTIONALLIGHTCOMPONENT_H_
#define _DIRECTIONALLIGHTCOMPONENT_H_

#include <core/LightComponent.h>

namespace Twin2Engine {
	namespace Core {
		class DirectionalLightComponent : public LightComponent {
			friend class GameObject;

			private:
				bool dirtyFlag = false;
				//glm::vec3 localDirection;
				LightingSystem::DirectionalLight* light = new LightingSystem::DirectionalLight;
				Twin2Engine::Core::Action<Transform*> OnChangePosition;
				size_t OnChangePositionId;
				Twin2Engine::Core::Method OnViewerChange;
				size_t OnViewerChangeId;
			//protected:
			public:
				DirectionalLightComponent() : LightComponent() {};

				virtual void Initialize() override;
				virtual void Update() override;
				virtual void OnEnable() override;
				virtual void OnDisable() override;
				virtual void OnDestroy() override;

				void SetDirection(glm::vec3 dir);
				void SetColor(glm::vec3 color);
				void SetPower(float power);

				virtual YAML::Node Serialize() const override;
		};
	}
}

#endif // !_DIRECTIONALLIGHTCOMPONENT_H_
