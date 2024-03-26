#ifndef _SPHERECOLLIDERCOMPONENT_H_
#define	_SPHERECOLLIDERCOMPONENT_H_

#include <core/ColliderComponent.h>

namespace Twin2Engine {
	namespace Core {
		class SphereColliderComponent : public ColliderComponent {
			//protected:

			public:
				SphereColliderComponent();
				void SetRadius(float radius);
		};
	}
}


#endif // !_SPHERECOLLIDERCOMPONENT_H_
