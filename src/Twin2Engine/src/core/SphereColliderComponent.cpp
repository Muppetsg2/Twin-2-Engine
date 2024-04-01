#include <core/SphereColliderComponent.h>
#include "CollisionManager.h"

Twin2Engine::Core::SphereColliderComponent::SphereColliderComponent() : ColliderComponent()
{
	CollisionSystem::SphereColliderData* colliderData = new CollisionSystem::SphereColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::SphereColliderComponent::SetRadius(float radius)
{
	((CollisionSystem::SphereColliderData*)collider->shapeColliderData)->Radius = radius;
}
