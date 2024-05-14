#pragma once

#include <physic/Collider.h>
#include <graphic/Frustum.h>

namespace Twin2Engine::Physic {
	class BoundingVolume : public Collider {
	public:
		BoundingVolume(SphereColliderData* sphereColliderData);
		BoundingVolume(BoxColliderData* boxColliderData);
		BoundingVolume(CapsuleColliderData* CapsuleColliderData);

		bool isOnFrustum(Graphic::Frustum & frustum);

		Collision* collide(Collider* other) override;
	};
}