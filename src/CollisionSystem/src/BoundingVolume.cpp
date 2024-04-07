#include "BoundingVolume.h"

using namespace CollisionSystem;

BoundingVolume::BoundingVolume(SphereColliderData* sphereColliderData) {
	colliderShape = ColliderShape::SPHERE;
	shapeColliderData = sphereColliderData;
}

BoundingVolume::BoundingVolume(BoxColliderData* boxColliderData) {
	colliderShape = ColliderShape::BOX;
	shapeColliderData = boxColliderData;
}

BoundingVolume::BoundingVolume(CapsuleColliderData* capsuleColliderData) {
	colliderShape = ColliderShape::CAPSULE;
	shapeColliderData = capsuleColliderData;
}

bool CollisionSystem::BoundingVolume::isOnFrustrum()
{
	return true;
}

Collision* BoundingVolume::collide(Collider* other) {
	/*if (other->isBoundingVolume) {
		return testCollision((Collider*)this, other, false);
	}
	else {

	}

	return nullptr;/**/
	return testCollision((Collider*)this, other, false);
};