#pragma once

namespace CollisionSystem {
	//struct Collision;
	class Collider;

	//typedef Collider* ColliderPtr;

	struct Collision {
		Collider* collider;
		Collider* otherCollider;
		//glm::vec3 position;
		glm::vec3 separation;
	};

	//typedef Collision* CollisionPtr;


	struct ShapeColliderData {
		glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 LocalPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	};

	struct SphereColliderData : ShapeColliderData {
		float Radius = 1.0f;
		float LocalRadius = 1.0f;
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


	class Collider {
	public:
		bool isBoundingVolume = false;
		enum ColliderShape : uint8_t { SPHERE, BOX, CAPSULE };

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

		static Collision* testCollision(Collider* collider, Collider* otherCollider, bool separate);
	};
}