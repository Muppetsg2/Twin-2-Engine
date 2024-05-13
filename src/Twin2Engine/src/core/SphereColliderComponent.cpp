#include <core/SphereColliderComponent.h>
#include <core/GameObject.h>
#include <physic/CollisionManager.h>
#include <tools/YamlConverters.h>
#include <physic/GameCollider.h>
#include <core/ColliderComponent.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

SphereColliderComponent::SphereColliderComponent() : ColliderComponent()
{
	collider = new GameCollider(this, new SphereColliderData());
}

void SphereColliderComponent::SetRadius(float radius)
{
	((SphereColliderData*)collider->shapeColliderData)->Radius = radius;
}

void SphereColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	PositionChangeAction = [this](Transform* transform) {
		collider->shapeColliderData->Position = transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	PositionChangeAction(GetTransform());
}

void SphereColliderComponent::OnEnable()
{
	PositionChangeActionId = GetTransform()->OnEventPositionChanged += PositionChangeAction;
	CollisionManager::Instance()->RegisterCollider(collider);
}

void SphereColliderComponent::OnDisable()
{
	GetTransform()->OnEventPositionChanged -= PositionChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

void SphereColliderComponent::OnDestroy()
{
	GetTransform()->OnEventPositionChanged -= PositionChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

YAML::Node SphereColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<std::string>());
	node["type"] = "SphereCollider";
	node["radius"] = ((SphereColliderData*)collider->shapeColliderData)->Radius;
	return node;
}
