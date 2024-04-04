#ifndef _CAPSULECOLLIDERCOMPONENT_H_
#define _CAPSULECOLLIDERCOMPONENT_H_

#include <core/ColliderComponent.h>

namespace Twin2Engine {
	namespace Core {
		class CapsuleColliderComponent : public ColliderComponent {
			//protected: 
			public:
				CapsuleColliderComponent();
				void SetEndPosition(float x, float y, float z);
				void SetRadius(float radius);

				void Update() override;
	
		};
	}
	
}

#endif // !_CAPSULECOLLIDERCOMPONENT_H_
