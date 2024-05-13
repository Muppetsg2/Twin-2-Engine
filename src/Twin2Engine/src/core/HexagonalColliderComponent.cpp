#include <core/HexagonalColliderComponent.h>

#include "core/GameObject.h"
#include <CollisionManager.h>
#include <core/YamlConverters.h>

Twin2Engine::Core::HexagonalColliderComponent::HexagonalColliderComponent() : ColliderComponent()
{
	collider = new CollisionSystem::GameCollider(this, new CollisionSystem::HexagonalColliderData());
}

void Twin2Engine::Core::HexagonalColliderComponent::SetBaseLength(float v)
{
	((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData)->BaseLength = v;
}

void Twin2Engine::Core::HexagonalColliderComponent::SetHalfHeight(float v)
{
	((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData)->HalfHeight = v;
}

void Twin2Engine::Core::HexagonalColliderComponent::SetYRotation(float v)
{
	((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData)->Rotation = v;
	dirtyFlag = true;
}

void Twin2Engine::Core::HexagonalColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		CollisionSystem::HexagonalColliderData* hexData = ((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData);
		glm::quat q = transform->GetGlobalRotationQuat() * glm::angleAxis(hexData->Rotation, glm::vec3(0.0f, 1.0f, 0.0f));
		hexData->u = q * glm::vec3(-0.5f, 0.0f, 0.866f);
		hexData->v = q * glm::vec3(0.5f, 0.0f, 0.866f);
		hexData->w = q * glm::vec3(1.0f, 0.0f, 0.0f);

		//hexData->HalfHeight = transform->GetGlobalScale().y * hexData->HalfHeight;

		collider->shapeColliderData->Position = transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	TransformChangeAction(GetTransform());
}

void Twin2Engine::Core::HexagonalColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::Update()
{
	Twin2Engine::Core::ColliderComponent::Update();

	if (dirtyFlag) {
		CollisionSystem::HexagonalColliderData * hexData = ((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData);
		glm::quat q = GetTransform()->GetGlobalRotationQuat() * glm::angleAxis(hexData->Rotation, glm::vec3(0.0f, 1.0f, 0.0f));
		hexData->u = q * glm::vec3(-0.5f, 0.0f, 0.866f);
		hexData->v = q * glm::vec3(0.5f, 0.0f, 0.866f);
		hexData->w = q * glm::vec3(1.0f, 0.0f, 0.0f);

		//hexData->HalfHeight = GetTransform()->GetGlobalScale().y * hexData->HalfHeight;

		dirtyFlag = false;
	}
}

YAML::Node Twin2Engine::Core::HexagonalColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<std::string>());
	node["type"] = "HexagonalCollider";
	node["baselength"] = ((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData)->BaseLength;
	node["halfheight"] = ((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData)->HalfHeight;
	node["rotation"] = ((CollisionSystem::HexagonalColliderData*)collider->shapeColliderData)->Rotation;
	return node;
}
