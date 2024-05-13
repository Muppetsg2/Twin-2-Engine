#include <core/HexagonalColliderComponent.h>
#include "core/GameObject.h"
#include <physic/CollisionManager.h>
#include <tools/YamlConverters.h>

Twin2Engine::Core::HexagonalColliderComponent::HexagonalColliderComponent() : ColliderComponent()
{
	collider = new Twin2Engine::Physic::GameCollider(this, new Twin2Engine::Physic::HexagonalColliderData());
}

void Twin2Engine::Core::HexagonalColliderComponent::SetBaseLength(float v)
{
	((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->BaseLength = v;
}

void Twin2Engine::Core::HexagonalColliderComponent::SetHalfHeight(float v)
{
	((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->HalfHeight = v;
}

void Twin2Engine::Core::HexagonalColliderComponent::SetYRotation(float v)
{
	((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->Rotation = v;
	dirtyFlag = true;
}

void Twin2Engine::Core::HexagonalColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		Twin2Engine::Physic::HexagonalColliderData* hexData = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData);
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
	Twin2Engine::Physic::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	Twin2Engine::Physic::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	Twin2Engine::Physic::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::Update()
{
	Twin2Engine::Core::ColliderComponent::Update();

	if (dirtyFlag) {
		Twin2Engine::Physic::HexagonalColliderData * hexData = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData);
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
	node["baselength"] = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->BaseLength;
	node["halfheight"] = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->HalfHeight;
	node["rotation"] = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->Rotation;
	return node;
}
