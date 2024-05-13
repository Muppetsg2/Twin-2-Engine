#ifndef _RAY_H_
#define _RAY_H_

namespace Twin2Engine::Core {
	class ColliderComponent;
}

namespace Twin2Engine::Physic {
	struct RaycastHit {
		Core::ColliderComponent* collider = nullptr;
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
