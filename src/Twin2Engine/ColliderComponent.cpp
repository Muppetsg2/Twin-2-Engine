#include <core/ColliderComponent.h>
#include <core/GameObject.h>
#include "CollisionManager.h"

using namespace Twin2Engine::Core;

ColliderComponent::ColliderComponent() : Component()
{
	CollisionSystem::SphereColliderData* bvData = new CollisionSystem::SphereColliderData();
	boundingVolume = new CollisionSystem::BoundingVolume(bvData);
	//CollisionManager::Instance->RegisterCollider(this);
	//colliderComponents.push_back(this);
}

ColliderComponent::~ColliderComponent()
{
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
	/*for (size_t i = 0; i < colliderComponents.size(); ++i) {
		if (colliderComponents[i] == this) {
			colliderComponents.erase(colliderComponents.begin() + i);
			break;
		}
	}*/
}


void ColliderComponent::SetTrigger(bool v)
{
	collider->isTrigger = v;
}

bool ColliderComponent::IsTrigger()
{
	return collider->isTrigger;
}

void ColliderComponent::SetStatic(bool v)
{
	collider->isStatic = v;
}

bool ColliderComponent::IsStatic()
{
	return collider->isStatic;
}

void ColliderComponent::SetLayer(Layer layer)
{
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
	collider->layer = layer;
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

Layer ColliderComponent::GetLayer()
{
	return collider->layer;
}

void ColliderComponent::SetLayersFilter(LayerCollisionFilter& layersFilter)
{
	collider->layersFilter = layersFilter;
}

void ColliderComponent::EnableBoundingVolume(bool v)
{
	if (v) {
		collider->boundingVolume = boundingVolume;
	}
	else {
		collider->boundingVolume = nullptr;
	}
}

void ColliderComponent::SetBoundingVolumeRadius(float radius)
{
	((CollisionSystem::SphereColliderData*)collider->boundingVolume)->Radius = radius;
}

void ColliderComponent::SetLocalPosition(float x, float y, float z)
{
	collider->shapeColliderData->LocalPosition.x = x;
	collider->shapeColliderData->LocalPosition.y = y;
	collider->shapeColliderData->LocalPosition.z = z;
}

void Twin2Engine::Core::ColliderComponent::Invoke()
{
	collider->colliderComponent = this;
}

void Twin2Engine::Core::ColliderComponent::Update()
{
	collider->shapeColliderData->Position = collider->shapeColliderData->LocalPosition + GetGameObject()->GetTransform()->GetGlobalPosition();
}


/*/#include <core/ColliderComponent.h>

using namespace Twin2Engine::Core;

ColliderComponent::ColliderComponent()
{
	colliderComponents.push_back(this);
}

ColliderComponent::~ColliderComponent()
{
	for (size_t i = 0; i < colliderComponents.size(); ++i) {
		if (colliderComponents[i] == this) {
			colliderComponents.erase(colliderComponents.begin() + i);
			break;
		}
	}
}/**/
