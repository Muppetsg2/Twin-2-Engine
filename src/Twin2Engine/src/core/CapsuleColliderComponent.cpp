#include <core/CapsuleColliderComponent.h>
#include <physics/CollisionManager.h>
#include <core/GameObject.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::PhysicsEngine;
using namespace Twin2Engine::Core;

CapsuleColliderComponent::CapsuleColliderComponent() : ColliderComponent()
{
	collider = new GameCollider(this, new CapsuleColliderData());
}

void CapsuleColliderComponent::SetEndPosition(float x, float y, float z)
{
	((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.x = x;
	((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.y = y;
	((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.z = z;
	dirtyFlag = true;
}

void CapsuleColliderComponent::SetRadius(float radius)
{
	((CapsuleColliderData*)collider->shapeColliderData)->Radius = radius;
}

void CapsuleColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		glm::mat4 TransformMatrix = transform->GetTransformMatrix();
		CapsuleColliderData* capsuleData = (CapsuleColliderData*)collider->shapeColliderData;
		capsuleData->EndPosition = TransformMatrix * glm::vec4(capsuleData->EndLocalPosition, 1.0f);
		//collider->shapeColliderData->Position = collider->shapeColliderData->LocalPosition + GetGameObject()->GetTransform()->GetGlobalPosition();
		capsuleData->Position = TransformMatrix * glm::vec4(capsuleData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	TransformChangeAction(GetTransform());
}

void CapsuleColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	CollisionManager::Instance()->RegisterCollider(collider);
}

void CapsuleColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

void CapsuleColliderComponent::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

void CapsuleColliderComponent::Update()
{
	ColliderComponent::Update();

	if (dirtyFlag) {
		((CapsuleColliderData*)collider->shapeColliderData)->EndPosition = GetTransform()->GetTransformMatrix()
			* glm::vec4(((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition, 1.0f);

		dirtyFlag = false;
	}
}

YAML::Node CapsuleColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<std::string>());
	node["type"] = "CapsuleCollider";
	node["endPosition"] = glm::vec3(
		((CapsuleColliderData*)collider->shapeColliderData)->EndPosition.x,
		((CapsuleColliderData*)collider->shapeColliderData)->EndPosition.y,
		((CapsuleColliderData*)collider->shapeColliderData)->EndPosition.z
	);
	node["radius"] = ((CapsuleColliderData*)collider->shapeColliderData)->Radius;
	return node;
}
