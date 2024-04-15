#ifndef _BOXCOLLIDERCOMPONENT_H_
#define _BOXCOLLIDERCOMPONENT_H_

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class BoxColliderComponent : public ColliderComponent {
		//protected:
	private:
		bool dirtyFlag = false;

		Twin2Engine::Core::Action<Transform*> TransformChangeAction;
		size_t TransformChangeActionId;

	public:
		BoxColliderComponent();
		//Along X
		void SetWidth(float v);
		//Along Z
		void SetLength(float v);
		//Along Y
		void SetHeight(float v);

		void SetXRotation(float v);
		void SetYRotation(float v);
		void SetZRotation(float v);

		void Initialize() override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDestroy() override;
		void Update() override;

		virtual YAML::Node Serialize() const override;
	};
}

#endif // !_BOXCOLLIDERCOMPONENT_H_
