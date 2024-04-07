#include <core/CapsuleColliderComponent.h>
#include <CollisionManager.h>

Twin2Engine::Core::CapsuleColliderComponent::CapsuleColliderComponent() : ColliderComponent()
{
	CollisionSystem::CapsuleColliderData* colliderData = new CollisionSystem::CapsuleColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::CapsuleColliderComponent::SetEndPosition(float x, float y, float z)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndPosition.x = x;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndPosition.y = y;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndPosition.z = z;
}

void Twin2Engine::Core::CapsuleColliderComponent::SetRadius(float radius)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->Radius = radius;
}
