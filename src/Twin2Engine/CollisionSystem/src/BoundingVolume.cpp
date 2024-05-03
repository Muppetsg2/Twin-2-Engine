#include "BoundingVolume.h"

using namespace CollisionSystem;

BoundingVolume::BoundingVolume(SphereColliderData* sphereColliderData) {
	colliderShape = ColliderShape::SPHERE;
	shapeColliderData = sphereColliderData;
	isBoundingVolume = true;
}

BoundingVolume::BoundingVolume(BoxColliderData* boxColliderData) {
	colliderShape = ColliderShape::BOX;
	shapeColliderData = boxColliderData;
	isBoundingVolume = true;
}

BoundingVolume::BoundingVolume(CapsuleColliderData* capsuleColliderData) {
	colliderShape = ColliderShape::CAPSULE;
	shapeColliderData = capsuleColliderData;
	isBoundingVolume = true;
}

//BoundingVolume::~BoundingVolume() {}

bool isOverPlane(FrustumPlane& plane, BoxColliderData* colliderData) {
	glm::vec3 relPos = plane.point - colliderData->Position;
	float distance = glm::dot(relPos, plane.normal);
	float width = colliderData->HalfDimensions.x * glm::abs(glm::dot(colliderData->XAxis, plane.normal))
		+ colliderData->HalfDimensions.y * glm::abs(glm::dot(colliderData->YAxis, plane.normal))
		+ colliderData->HalfDimensions.z * glm::abs(glm::dot(colliderData->ZAxis, plane.normal));
	return (glm::abs(distance) < width) || ((distance) < 0);
}

bool isOverPlane(FrustumPlane& plane, SphereColliderData* colliderData) {
	glm::vec3 relPos = colliderData->Position - plane.point;
	float distance = glm::dot(relPos, plane.normal);

	return distance > -colliderData->Radius;
	//return (glm::abs(distance) < colliderData->Radius) || ((distance) < 0);
}

bool isOverPlane(FrustumPlane& plane, CapsuleColliderData* colliderData) {
	float startDist = glm::dot(plane.normal, colliderData->Position) - glm::dot(plane.normal, plane.point);
	float endDist = glm::dot(plane.normal, colliderData->EndPosition) - glm::dot(plane.normal, plane.point);
	float minDist = glm::min(startDist, endDist);
	float maxDist = max(startDist, endDist);
	return minDist <= colliderData->Radius && maxDist >= (-colliderData->Radius);
}

bool CollisionSystem::BoundingVolume::isOnFrustum(Frustum& frustum)
{
	switch (colliderShape) {
		case (ColliderShape::BOX):
			return isOverPlane(frustum.bottomFace, (BoxColliderData*)shapeColliderData) && isOverPlane(frustum.topFace, (BoxColliderData*)shapeColliderData)
				&& isOverPlane(frustum.leftFace, (BoxColliderData*)shapeColliderData) && isOverPlane(frustum.rightFace, (BoxColliderData*)shapeColliderData)
				&& isOverPlane(frustum.farFace, (BoxColliderData*)shapeColliderData) && isOverPlane(frustum.nearFace, (BoxColliderData*)shapeColliderData);

		case (ColliderShape::SPHERE):
			return isOverPlane(frustum.bottomFace, (SphereColliderData*)shapeColliderData) && isOverPlane(frustum.topFace, (SphereColliderData*)shapeColliderData)
				&& isOverPlane(frustum.leftFace, (SphereColliderData*)shapeColliderData) && isOverPlane(frustum.rightFace, (SphereColliderData*)shapeColliderData)
				&& isOverPlane(frustum.farFace, (SphereColliderData*)shapeColliderData) && isOverPlane(frustum.nearFace, (SphereColliderData*)shapeColliderData);

		case (ColliderShape::CAPSULE):
			return isOverPlane(frustum.bottomFace, (CapsuleColliderData*)shapeColliderData) && isOverPlane(frustum.topFace, (CapsuleColliderData*)shapeColliderData)
				&& isOverPlane(frustum.leftFace, (CapsuleColliderData*)shapeColliderData) && isOverPlane(frustum.rightFace, (CapsuleColliderData*)shapeColliderData)
				&& isOverPlane(frustum.farFace, (CapsuleColliderData*)shapeColliderData) && isOverPlane(frustum.nearFace, (CapsuleColliderData*)shapeColliderData);

	}
	return false;
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