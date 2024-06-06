#pragma once

#include <physic/Collider.h>
#include <graphic/Frustum.h>

namespace Twin2Engine::Physic {
	ENUM_CLASS_BASE_VALUE(BoundingShape, uint8_t, BOX, 0, SPHERE, 1, CAPSULE, 2);

	class BoundingVolume : public Collider {
	public:

		BoundingVolume(BoundingShape boundingData);
		/*
		BoundingVolume(SphereColliderData* sphereColliderData);
		BoundingVolume(BoxColliderData* boxColliderData);
		BoundingVolume(CapsuleColliderData* CapsuleColliderData);
		*/

		~BoundingVolume() override;

		bool isOnFrustum(Graphic::Frustum & frustum);

		Collision* collide(Collider* other) override;
	};
}