#include <core/CapsuleColliderComponent.h>
#include <core/GameObject.h>
#include "CollisionManager.h"

Twin2Engine::Core::CapsuleColliderComponent::CapsuleColliderComponent() : ColliderComponent()
{
	CollisionSystem::CapsuleColliderData* colliderData = new CollisionSystem::CapsuleColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::CapsuleColliderComponent::SetEndPosition(float x, float y, float z)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.x = x;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.y = y;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.z = z;
	//dirtyFlag = true;
}

void Twin2Engine::Core::CapsuleColliderComponent::SetRadius(float radius)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->Radius = radius;
}

void Twin2Engine::Core::CapsuleColliderComponent::Update()
{
	//bool dirtyFlag = true;
	//collider->shapeColliderData->Position = collider->shapeColliderData->LocalPosition + getGameObject()->GetTransform()->GetGlobalPosition();
	if (dirtyFlag) {
		((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndPosition = 
												GetGameObject()->GetTransform()->GetTransformMatrix()
											  * glm::vec4(((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition, 1.0f);

		Twin2Engine::Core::ColliderComponent::Update();
	}
}
