#pragma once

//#define BoundingVolumePtr BoundingVolume*
#include <physics/Collider.h>
#include <graphic/Frustum.h>

namespace Twin2Engine::PhysicsEngine {
	class BoundingVolume : public Collider {
	public:
		BoundingVolume(SphereColliderData* sphereColliderData);
		BoundingVolume(BoxColliderData* boxColliderData);
		BoundingVolume(CapsuleColliderData* CapsuleColliderData);

		//virtual ~BoundingVolume();

		bool isOnFrustum(GraphicEngine::Frustum & frustum);

		Collision* collide(Collider* other) override;
	};
}

//typedef BoundingVolume* BoundingVolumePtr;