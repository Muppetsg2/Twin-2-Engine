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

		auto Pos = transform->GetGlobalPosition();
		SPDLOG_INFO("{}.  Pos: \t\t{}\t{}\t{}", colliderId, Pos.x, Pos.y, Pos.z);
		SPDLOG_INFO("{}.  ColPos: \t{}\t{}\t{}\t\t\t\tLocPos: \t{}\t{}\t{}", colliderId, collider->shapeColliderData->Position.x, collider->shapeColliderData->Position.y, collider->shapeColliderData->Position.z, collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);
		//collider->shapeColliderData->Position = glm::vec3(transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f));
		//auto TM = transform->GetTransformMatrix();
		collider->shapeColliderData->Position = glm::vec3(transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f));
		SPDLOG_INFO("{}.  NewPos: \t{}\t{}\t{}\t\t\t\tLocPos: \t{}\t{}\t{}", colliderId, collider->shapeColliderData->Position.x, collider->shapeColliderData->Position.y, collider->shapeColliderData->Position.z, collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	//TransformChangeAction(GetTransform());
}

void Twin2Engine::Core::HexagonalColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	Twin2Engine::Physic::CollisionManager* cm = Twin2Engine::Physic::CollisionManager::Instance();
	cm->RegisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	Twin2Engine::Physic::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::HexagonalColliderComponent::OnDestroy()
{
	//GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
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
	node["type"] = "HexagonalCollider";
	node["baseLength"] = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->BaseLength;
	node["halfHeight"] = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->HalfHeight;
	node["rotation"] = ((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->Rotation;
	return node;
}

bool Twin2Engine::Core::HexagonalColliderComponent::Deserialize(const YAML::Node& node)
{
	if (!node["baseLength"] || !node["halfHeight"] || !node["rotation"] ||
		!ColliderComponent::Deserialize(node)) return false;

	((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->BaseLength = node["baseLength"].as<float>();
	((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->HalfHeight = node["halfHeight"].as<float>();
	((Twin2Engine::Physic::HexagonalColliderData*)collider->shapeColliderData)->Rotation = node["rotation"].as<float>();

	return true;
}
