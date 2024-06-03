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
	node["type"] = "SphereCollider";

	if (collider != nullptr) {
		node["radius"] = ((SphereColliderData*)collider->shapeColliderData)->Radius;
	}
	else {
		node["radius"] = 0.f;
	}

	return node;
}

bool SphereColliderComponent::Deserialize(const YAML::Node& node)
{
	if (!node["radius"] || !ColliderComponent::Deserialize(node)) return false;

	((SphereColliderData*)collider->shapeColliderData)->Radius = node["radius"].as<float>();

	return true;
}

#if _DEBUG
void SphereColliderComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Sphere Collider##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;
		ImGui::DragFloat(string("Radius##").append(id).c_str(), &((SphereColliderData*)collider->shapeColliderData)->Radius, 0.1f);

		DrawInheritedFields();
	}
}
#endif