#include "core/BoxColliderComponent.h"
#include "core/GameObject.h"
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

void Twin2Engine::Core::BoxColliderComponent::Update()
{
	if (dirtyFlag) {
		glm::mat4 r = glm::mat4(1.0f);
		CollisionSystem::BoxColliderData* boxData = ((CollisionSystem::BoxColliderData*)collider->shapeColliderData);
		r = glm::rotate(r, boxData->Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		r = glm::rotate(r, boxData->Rotation.y, glm::vec3(1.0f, 1.0f, 0.0f));
		r = glm::rotate(r, boxData->Rotation.z, glm::vec3(1.0f, 0.0f, 1.0f));
		//((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation
		r = GetGameObject()->GetTransform()->GetTransformMatrix() * r;
		
		boxData->XAxis = r * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		boxData->YAxis = r * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		boxData->ZAxis = r * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		Twin2Engine::Core::ColliderComponent::Update();
	}
}
