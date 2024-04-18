#include <core/SphereColliderComponent.h>
#include <core/GameObject.h>
#include <CollisionManager.h>
#include <tools/YamlConverters.h>
#include <GameCollider.h>
#include <core/ColliderComponent.h>


Twin2Engine::Core::SphereColliderComponent::SphereColliderComponent() : ColliderComponent()
{
	collider = new CollisionSystem::GameCollider(this, new CollisionSystem::SphereColliderData());
}

void Twin2Engine::Core::SphereColliderComponent::SetRadius(float radius)
{
	((CollisionSystem::SphereColliderData*)collider->shapeColliderData)->Radius = radius;
}

void Twin2Engine::Core::SphereColliderComponent::Initialize()
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

void Twin2Engine::Core::SphereColliderComponent::OnEnable()
{
	PositionChangeActionId = GetTransform()->OnEventPositionChanged += PositionChangeAction;
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::SphereColliderComponent::OnDisable()
{
	GetTransform()->OnEventPositionChanged -= PositionChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::SphereColliderComponent::OnDestroy()
{
	GetTransform()->OnEventPositionChanged -= PositionChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

YAML::Node Twin2Engine::Core::SphereColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<std::string>());
	node["type"] = "SphereCollider";
	node["radius"] = ((CollisionSystem::SphereColliderData*)collider->shapeColliderData)->Radius;
	return node;
}
