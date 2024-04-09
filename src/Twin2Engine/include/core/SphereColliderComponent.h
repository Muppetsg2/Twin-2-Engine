#ifndef _SPHERECOLLIDERCOMPONENT_H_
#define	_SPHERECOLLIDERCOMPONENT_H_

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class SphereColliderComponent : public ColliderComponent {
		//protected:
	private:
		Twin2Engine::Core::Action<Transform*> PositionChangeAction;
		size_t PositionChangeActionId;
	public:
		SphereColliderComponent() : ColliderComponent() {};
		void SetRadius(float radius);

		void Initialize() override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDestroy() override;
	};
}


#endif // !_SPHERECOLLIDERCOMPONENT_H_
