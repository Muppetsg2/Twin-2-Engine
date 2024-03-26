#ifndef _BOUNDINGVOLUME_H_
#define _BOUNDINGVOLUME_H_

//#define BoundingVolumePtr BoundingVolume*
#include "Collider.h"

namespace CollisionSystem {
	class BoundingVolume : public Collider {
		public:
			BoundingVolume(SphereColliderData* sphereColliderData);
			BoundingVolume(BoxColliderData* boxColliderData);
			BoundingVolume(CapsuleColliderData* CapsuleColliderData);
	
			//virtual ~BoundingVolume();
	
			bool isOnFrustrum();

			Collision* collide(Collider* other) override;
	};
}



//typedef BoundingVolume* BoundingVolumePtr;

#endif // !_BOUNDINGVOLUME_H_
