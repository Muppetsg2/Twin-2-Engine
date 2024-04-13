#ifndef _SPHERECOLLIDERCOMPONENT_H_
#define	_SPHERECOLLIDERCOMPONENT_H_

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class SphereColliderComponent : public ColliderComponent {
		//protected:

	public:
		SphereColliderComponent();
		void SetRadius(float radius);

		virtual YAML::Node Serialize() const override;
	};
}


#endif // !_SPHERECOLLIDERCOMPONENT_H_
