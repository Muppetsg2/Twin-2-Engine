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
				LightingSystem::DirectionalLight* light = nullptr;
				Twin2Engine::Core::Action<Transform*> OnChangePosition;
				size_t OnChangePositionId;
				Twin2Engine::Core::Action<> OnViewerChange;
				size_t OnViewerChangeId;
			protected:
				DirectionalLightComponent() : LightComponent() {};
			public:

				virtual void Initialize() override;
				virtual void Update() override;
				virtual void OnEnable() override;
				virtual void OnDisable() override;
				virtual void OnDestroy() override;

				void SetDirection(glm::vec3 dir);
				void SetColor(glm::vec3 color);
				void SetPower(float power);
		};
	}
}

#endif // !_DIRECTIONALLIGHTCOMPONENT_H_
