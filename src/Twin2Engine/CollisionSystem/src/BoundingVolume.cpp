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

bool isOverPlane(FrustumPlane& plane, BoxColliderData* colliderData) {
	glm::vec3 relPos = plane.point - colliderData->Position;
	float distance = glm::dot(relPos, plane.normal);
	float width = colliderData->HalfDimensions.x * glm::abs(glm::dot(colliderData->XAxis, plane.normal))
		+ colliderData->HalfDimensions.y * glm::abs(glm::dot(colliderData->YAxis, plane.normal))
		+ colliderData->HalfDimensions.z * glm::abs(glm::dot(colliderData->ZAxis, plane.normal));
	return (glm::abs(distance) < width) || ((distance) < 0);
}

bool isOverPlane(FrustumPlane& plane, SphereColliderData* colliderData) {
	glm::vec3 relPos = plane.point - colliderData->Position;
	float distance = glm::dot(relPos, plane.normal);

	return (glm::abs(distance) < colliderData->Radius) || ((distance) < 0);
}

bool isOverPlane(FrustumPlane& plane, CapsuleColliderData* colliderData) {
	float startDist = glm::dot(plane.normal, colliderData->Position) - glm::dot(plane.normal, plane.point);
	float endDist = glm::dot(plane.normal, colliderData->EndPosition) - glm::dot(plane.normal, plane.point);
	float minDist = glm::min(startDist, endDist);
	float maxDist = max(startDist, endDist);
	return minDist <= colliderData->Radius && maxDist >= (-colliderData->Radius);
}

bool CollisionSystem::BoundingVolume::isOnFrustum(Frustum* frustum)
{
	switch (colliderShape) {
	case (ColliderShape::BOX):
		return isOverPlane(frustum->bottomFace, (BoxColliderData*)colliderShape) && isOverPlane(frustum->topFace, (BoxColliderData*)colliderShape)
			&& isOverPlane(frustum->leftFace, (BoxColliderData*)colliderShape) && isOverPlane(frustum->rightFace, (BoxColliderData*)colliderShape)
			&& isOverPlane(frustum->farFace, (BoxColliderData*)colliderShape) && isOverPlane(frustum->nearFace, (BoxColliderData*)colliderShape);

	case (ColliderShape::SPHERE):
		return isOverPlane(frustum->bottomFace, (SphereColliderData*)colliderShape) && isOverPlane(frustum->topFace, (SphereColliderData*)colliderShape)
			&& isOverPlane(frustum->leftFace, (SphereColliderData*)colliderShape) && isOverPlane(frustum->rightFace, (SphereColliderData*)colliderShape)
			&& isOverPlane(frustum->farFace, (SphereColliderData*)colliderShape) && isOverPlane(frustum->nearFace, (SphereColliderData*)colliderShape);

	case (ColliderShape::CAPSULE):
		return isOverPlane(frustum->bottomFace, (CapsuleColliderData*)colliderShape) && isOverPlane(frustum->topFace, (CapsuleColliderData*)colliderShape)
			&& isOverPlane(frustum->leftFace, (CapsuleColliderData*)colliderShape) && isOverPlane(frustum->rightFace, (CapsuleColliderData*)colliderShape)
			&& isOverPlane(frustum->farFace, (CapsuleColliderData*)colliderShape) && isOverPlane(frustum->nearFace, (CapsuleColliderData*)colliderShape);

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