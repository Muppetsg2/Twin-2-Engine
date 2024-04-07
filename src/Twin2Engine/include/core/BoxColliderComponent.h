#ifndef _BOXCOLLIDERCOMPONENT_H_
#define _BOXCOLLIDERCOMPONENT_H_

#include <core/ColliderComponent.h>

namespace Twin2Engine::Core {
	class BoxColliderComponent : public ColliderComponent {
		//protected:

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

		void Update();
	};
}

#endif // !_BOXCOLLIDERCOMPONENT_H_
