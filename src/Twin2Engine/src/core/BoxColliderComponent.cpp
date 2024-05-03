#include "core/GameObject.h"
#include <core/BoxColliderComponent.h>
#include <physic/CollisionManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

BoxColliderComponent::BoxColliderComponent() : ColliderComponent()
{
	collider = new GameCollider(this, new BoxColliderData());
}

void BoxColliderComponent::SetWidth(float v)
{
	((BoxColliderData*)collider->shapeColliderData)->HalfDimensions.x = v;
}

void BoxColliderComponent::SetLength(float v)
{
	((BoxColliderData*)collider->shapeColliderData)->HalfDimensions.z = v;
}

void BoxColliderComponent::SetHeight(float v)
{
	((BoxColliderData*)collider->shapeColliderData)->HalfDimensions.y = v;
}

void BoxColliderComponent::SetXRotation(float v)
{
	((BoxColliderData*)collider->shapeColliderData)->Rotation.x = v;
	dirtyFlag = true;
}

void BoxColliderComponent::SetYRotation(float v)
{
	((BoxColliderData*)collider->shapeColliderData)->Rotation.y = v;
	dirtyFlag = true;
}

void BoxColliderComponent::SetZRotation(float v)
{
	((BoxColliderData*)collider->shapeColliderData)->Rotation.z = v;
	dirtyFlag = true;
}

void BoxColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		BoxColliderData* boxData = ((BoxColliderData*)collider->shapeColliderData);
		glm::quat q = transform->GetGlobalRotationQuat()
			* glm::angleAxis(boxData->Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::angleAxis(boxData->Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(boxData->Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		boxData->XAxis = q * glm::vec3(1.0f, 0.0f, 0.0f);
		boxData->YAxis = q * glm::vec3(0.0f, 1.0f, 0.0f);
		boxData->ZAxis = q * glm::vec3(0.0f, 0.0f, 1.0f);

		boxData->HalfDimensions = transform->GetGlobalScale() * boxData->LocalHalfDimensions;

		collider->shapeColliderData->Position = transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	TransformChangeAction(GetTransform());
}

void BoxColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	CollisionManager::Instance()->RegisterCollider(collider);
}

void BoxColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

void BoxColliderComponent::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

void BoxColliderComponent::Update()
{
	ColliderComponent::Update();

	if (dirtyFlag) {
		BoxColliderData* boxData = ((BoxColliderData*)collider->shapeColliderData);
		glm::quat q = GetTransform()->GetGlobalRotationQuat()
			* glm::angleAxis(boxData->Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::angleAxis(boxData->Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(boxData->Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		boxData->XAxis = q * glm::vec3(1.0f, 0.0f, 0.0f);
		boxData->YAxis = q * glm::vec3(0.0f, 1.0f, 0.0f);
		boxData->ZAxis = q * glm::vec3(0.0f, 0.0f, 1.0f);

		boxData->HalfDimensions = GetTransform()->GetGlobalScale() * boxData->LocalHalfDimensions;

		dirtyFlag = false;
	}
}

YAML::Node BoxColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<std::string>());
	node["type"] = "BoxCollider";
	node["width"] = ((BoxColliderData*)collider->shapeColliderData)->HalfDimensions.x;
	node["length"] = ((BoxColliderData*)collider->shapeColliderData)->HalfDimensions.z;
	node["height"] = ((BoxColliderData*)collider->shapeColliderData)->HalfDimensions.y;
	node["rotation"] = glm::vec3(
		((BoxColliderData*)collider->shapeColliderData)->Rotation.x,
		((BoxColliderData*)collider->shapeColliderData)->Rotation.y,
		((BoxColliderData*)collider->shapeColliderData)->Rotation.z
	);
	return node;
}
