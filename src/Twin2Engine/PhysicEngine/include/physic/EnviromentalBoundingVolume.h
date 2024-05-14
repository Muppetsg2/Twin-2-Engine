#ifndef _ENVIROMENTALBOUNDINGVOLUME_H_
#define _ENVIROMENTALBOUNDINGVOLUME_H_

#include <physic/BoundingVolume.h>


namespace Twin2Engine::Physic {
	class EnviromentalBoundingVolume : public BoundingVolume {
	public:
		std::vector<EnviromentalBoundingVolume*> ChildrenBoundingVolume;

		EnviromentalBoundingVolume(SphereColliderData* sphereColliderData);
		EnviromentalBoundingVolume(BoxColliderData* boxColliderData);
		EnviromentalBoundingVolume(CapsuleColliderData* capsuleColliderData);
	};
}

#endif // !_ENVIROMENTALBOUNDINGVOLUME_H_
