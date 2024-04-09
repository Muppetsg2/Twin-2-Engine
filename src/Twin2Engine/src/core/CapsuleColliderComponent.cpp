#include <core/CapsuleColliderComponent.h>
#include <CollisionManager.h>
#include <core/GameObject.h>


void Twin2Engine::Core::CapsuleColliderComponent::SetEndPosition(float x, float y, float z)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.x = x;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.y = y;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.z = z;
	//dirtyFlag = true;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndPosition = GetTransform()->GetTransformMatrix()
		* glm::vec4(((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition, 1.0f);
}

void Twin2Engine::Core::CapsuleColliderComponent::SetRadius(float radius)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->Radius = radius;
}

void Twin2Engine::Core::CapsuleColliderComponent::Initialize()
{
	CollisionSystem::CapsuleColliderData* colliderData = new CollisionSystem::CapsuleColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);

	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		glm::mat4 TransformMatrix = transform->GetTransformMatrix();
		((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndPosition = TransformMatrix
							* glm::vec4(((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition, 1.0f);
		//collider->shapeColliderData->Position = collider->shapeColliderData->LocalPosition + GetGameObject()->GetTransform()->GetGlobalPosition();
		collider->shapeColliderData->Position = TransformMatrix * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	TransformChangeAction(GetTransform());
}

void Twin2Engine::Core::CapsuleColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::CapsuleColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::CapsuleColliderComponent::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}
