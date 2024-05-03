#include <physic/EnviromentalBoundingVolume.h>

using namespace Twin2Engine::Physic;

EnviromentalBoundingVolume::EnviromentalBoundingVolume(SphereColliderData* sphereColliderData) : BoundingVolume(sphereColliderData) {

}

EnviromentalBoundingVolume::EnviromentalBoundingVolume(BoxColliderData* boxColliderData) : BoundingVolume(boxColliderData) {

}

EnviromentalBoundingVolume::EnviromentalBoundingVolume(CapsuleColliderData* capsuleColliderData) : BoundingVolume(capsuleColliderData) {

}