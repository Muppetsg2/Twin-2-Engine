#include "EnviromentalBoundingVolume.h"

using namespace CollisionSystem;

EnviromentalBoundingVolume::EnviromentalBoundingVolume(SphereColliderData* sphereColliderData) : BoundingVolume(sphereColliderData) {

}

EnviromentalBoundingVolume::EnviromentalBoundingVolume(BoxColliderData* boxColliderData) : BoundingVolume(boxColliderData) {

}

EnviromentalBoundingVolume::EnviromentalBoundingVolume(CapsuleColliderData* capsuleColliderData) : BoundingVolume(capsuleColliderData) {

}