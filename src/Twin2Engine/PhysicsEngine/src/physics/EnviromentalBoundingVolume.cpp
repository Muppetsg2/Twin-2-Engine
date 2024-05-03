#include <physics/EnviromentalBoundingVolume.h>

using namespace Twin2Engine::PhysicsEngine;

EnviromentalBoundingVolume::EnviromentalBoundingVolume(SphereColliderData* sphereColliderData) : BoundingVolume(sphereColliderData) {

}

EnviromentalBoundingVolume::EnviromentalBoundingVolume(BoxColliderData* boxColliderData) : BoundingVolume(boxColliderData) {

}

EnviromentalBoundingVolume::EnviromentalBoundingVolume(CapsuleColliderData* capsuleColliderData) : BoundingVolume(capsuleColliderData) {

}