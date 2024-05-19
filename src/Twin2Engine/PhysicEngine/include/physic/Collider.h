#pragma once

namespace Twin2Engine::Physic {
	class Collider;

	struct Collision {
		Collider* collider;
		Collider* otherCollider;
		glm::vec3 separation;
	};

	struct ShapeColliderData {
		glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 LocalPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	};

	struct SphereColliderData : ShapeColliderData {
		float Radius = 1.0f;
	};

	struct BoxColliderData : ShapeColliderData {
		glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 HalfDimensions = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 LocalHalfDimensions = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 XAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 YAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 ZAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	};

	struct CapsuleColliderData : ShapeColliderData {
		glm::vec3 EndPosition = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 EndLocalPosition = glm::vec3(0.0f, 1.0f, 0.0f);
		float Radius = 0.5f;
		float LocalRadius = 0.5f;
	};

	struct HexagonalColliderData : ShapeColliderData {
		float Rotation = 0.0f;
		float HalfHeight = 0.5f;
		float BaseLength = 0.5f;
		glm::vec3 u = glm::vec3(-0.5f, 0.0f, 0.866f);
		glm::vec3 v = glm::vec3(0.5f, 0.0f, 0.866f);
		glm::vec3 w = glm::vec3(1.0f, 0.0f, 0.0f);
	};
	
	ENUM_CLASS_BASE_VALUE(ColliderShape, uint8_t, SPHERE, 0, BOX, 1, CAPSULE, 2, HEXAGONAL, 4);
	/*
	enum class ColliderShape : uint8_t {
		SPHERE = 0,
		BOX = 1,
		CAPSULE = 2,
		HEXAGONAL = 4
	};
	*/

	class Collider {
	public:
		bool isBoundingVolume = false;

		ColliderShape colliderShape;
		ShapeColliderData* shapeColliderData;

		virtual ~Collider();

		virtual Collision* collide(Collider* other) = 0;

		static Collision* SphereSphereCollision(Collider* sphere1, Collider* sphere2, bool separate);
		static Collision* BoxBoxCollision(Collider* box1, Collider* box2, bool separate);
		static Collision* CapsuleCapsuleCollision(Collider* capsule1, Collider* capsule2, bool separate);
		static Collision* SphereBoxCollision(Collider* sphere, Collider* box, bool separate);
		static Collision* SphereCapsuleCollision(Collider* sphere, Collider* capsule, bool separate);
		static Collision* BoxCapsuleCollision(Collider* box, Collider* capsule, bool separate);
		static Collision* HexagonalSphereCollision(Collider* Hexagonal, Collider* sphere, bool separate);

		static Collision* testCollision(Collider* collider, Collider* otherCollider, bool separate);
	};
}