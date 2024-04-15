#include "core/BoxColliderComponent.h"
#include "core/GameObject.h"
#include "CollisionManager.h"


Twin2Engine::Core::BoxColliderComponent::BoxColliderComponent() : ColliderComponent()
{
	CollisionSystem::BoxColliderData* colliderData = new CollisionSystem::BoxColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);
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
	dirtyFlag = true;
}

void Twin2Engine::Core::BoxColliderComponent::SetYRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.y = v;
	dirtyFlag = true;
}

void Twin2Engine::Core::BoxColliderComponent::SetZRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.z = v;
	dirtyFlag = true;
}

void Twin2Engine::Core::BoxColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		/*//SPDLOG_INFO("{} BX TransformChangeAction!", colliderId);
		glm::mat4 TransformMatrix = glm::mat4(1.0f);
		CollisionSystem::BoxColliderData* boxData = ((CollisionSystem::BoxColliderData*)collider->shapeColliderData);
		TransformMatrix = glm::rotate(TransformMatrix, boxData->Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		TransformMatrix = glm::rotate(TransformMatrix, boxData->Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		TransformMatrix = glm::rotate(TransformMatrix, boxData->Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		//((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation
		TransformMatrix = transform->GetTransformMatrix() * TransformMatrix;
		transform->GetGlobalRotationQuat().;
		boxData->XAxis = TransformMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		boxData->YAxis = TransformMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		boxData->ZAxis = TransformMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);*/
		CollisionSystem::BoxColliderData* boxData = ((CollisionSystem::BoxColliderData*)collider->shapeColliderData);
		glm::quat q = transform->GetGlobalRotationQuat()
			* glm::angleAxis(boxData->Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::angleAxis(boxData->Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(boxData->Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		boxData->XAxis = q * glm::vec3(1.0f, 0.0f, 0.0f);
		boxData->YAxis = q * glm::vec3(0.0f, 1.0f, 0.0f);
		boxData->ZAxis = q * glm::vec3(0.0f, 0.0f, 1.0f);

		collider->shapeColliderData->Position = transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	TransformChangeAction(GetTransform());
}

void Twin2Engine::Core::BoxColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::BoxColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::BoxColliderComponent::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::BoxColliderComponent::Update()
{
	Twin2Engine::Core::ColliderComponent::Update();

	if (dirtyFlag) {
		glm::mat4 TransformMatrix = glm::mat4(1.0f);
		CollisionSystem::BoxColliderData* boxData = ((CollisionSystem::BoxColliderData*)collider->shapeColliderData);
		TransformMatrix = glm::rotate(TransformMatrix, boxData->Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		TransformMatrix = glm::rotate(TransformMatrix, boxData->Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		TransformMatrix = glm::rotate(TransformMatrix, boxData->Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		//((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation
		TransformMatrix = GetTransform()->GetTransformMatrix() * TransformMatrix;

		boxData->XAxis = TransformMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		boxData->YAxis = TransformMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		boxData->ZAxis = TransformMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		dirtyFlag = false;
	}
}
