#ifndef _RAY_H_
#define _RAY_H_

#include <core/ColliderComponent.h>


namespace Twin2Engine {
	namespace Core {
		class ColliderComponent;
	}
}

namespace CollisionSystem {
	struct RaycastHit {
		Twin2Engine::Core::ColliderComponent* collider = nullptr;
		glm::vec3 position;
	};

	class Ray {
	public:
		glm::vec3 Direction;
		glm::vec3 Origin;
		Ray(glm::vec3&& Direction, glm::vec3&& Origin);
	};
}


#endif // !_RAY_H_
