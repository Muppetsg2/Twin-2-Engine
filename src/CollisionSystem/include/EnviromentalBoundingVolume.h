#ifndef _ENVIROMENTALBOUNDINGVOLUME_H_
#define _ENVIROMENTALBOUNDINGVOLUME_H_

#include "BoundingVolume.h"


namespace CollisionSystem {
	class EnviromentalBoundingVolume : public BoundingVolume {
		public:
			std::vector<EnviromentalBoundingVolume*> ChildrenBoundingVolume;
	
			EnviromentalBoundingVolume(SphereColliderData* sphereColliderData);
			EnviromentalBoundingVolume(BoxColliderData* boxColliderData);
			EnviromentalBoundingVolume(CapsuleColliderData* capsuleColliderData);
	};
}

#endif // !_ENVIROMENTALBOUNDINGVOLUME_H_
