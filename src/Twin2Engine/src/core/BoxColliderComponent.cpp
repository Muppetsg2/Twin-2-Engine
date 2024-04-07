#include "core/BoxColliderComponent.h"
#include "CollisionManager.h"

Twin2Engine::Core::BoxColliderComponent::BoxColliderComponent() : ColliderComponent()
{
	CollisionSystem::BoxColliderData* colliderData = new CollisionSystem::BoxColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::BoxColliderComponent::SetWidth(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.x = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetLength(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.z = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetHeight(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.y = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetXRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.x = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetYRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.y = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetZRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.z = v;
}
