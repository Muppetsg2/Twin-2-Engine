#ifndef _ENVIROMENTALBOUNDINGVOLUME_H_
#define _ENVIROMENTALBOUNDINGVOLUME_H_

#include <physics/BoundingVolume.h>


namespace Twin2Engine::PhysicsEngine {
	class EnviromentalBoundingVolume : public BoundingVolume {
	public:
		std::vector<EnviromentalBoundingVolume*> ChildrenBoundingVolume;

		EnviromentalBoundingVolume(SphereColliderData* sphereColliderData);
		EnviromentalBoundingVolume(BoxColliderData* boxColliderData);
		EnviromentalBoundingVolume(CapsuleColliderData* capsuleColliderData);
	};
}

#endif // !_ENVIROMENTALBOUNDINGVOLUME_H_
