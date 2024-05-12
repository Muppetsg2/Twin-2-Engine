#include <core/CapsuleColliderComponent.h>
#include <CollisionManager.h>
#include <core/GameObject.h>
#include <core/YamlConverters.h>

Twin2Engine::Core::CapsuleColliderComponent::CapsuleColliderComponent() : ColliderComponent()
{
	collider = new CollisionSystem::GameCollider(this, new CollisionSystem::CapsuleColliderData());
}

void Twin2Engine::Core::CapsuleColliderComponent::SetEndPosition(float x, float y, float z)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.x = x;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.y = y;
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.z = z;
	dirtyFlag = true;
}

void Twin2Engine::Core::CapsuleColliderComponent::SetEndPosition(const glm::vec3& pos)
{
	SetEndPosition(pos.x, pos.y, pos.z);
}

void Twin2Engine::Core::CapsuleColliderComponent::SetRadius(float radius)
{
	((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->Radius = radius;
}

void Twin2Engine::Core::CapsuleColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		glm::mat4 TransformMatrix = transform->GetTransformMatrix();
		CollisionSystem::CapsuleColliderData* capsuleData = (CollisionSystem::CapsuleColliderData*)collider->shapeColliderData;
		capsuleData->EndPosition = TransformMatrix * glm::vec4(capsuleData->EndLocalPosition, 1.0f);
		//collider->shapeColliderData->Position = collider->shapeColliderData->LocalPosition + GetGameObject()->GetTransform()->GetGlobalPosition();
		capsuleData->Position = TransformMatrix * glm::vec4(capsuleData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	TransformChangeAction(GetTransform());
}

void Twin2Engine::Core::CapsuleColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::CapsuleColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::CapsuleColliderComponent::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
}

void Twin2Engine::Core::CapsuleColliderComponent::Update()
{
	Twin2Engine::Core::ColliderComponent::Update();

	if (dirtyFlag) {
		((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndPosition = GetTransform()->GetTransformMatrix()
			* glm::vec4(((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition, 1.0f);

		dirtyFlag = false;
	}
}

YAML::Node Twin2Engine::Core::CapsuleColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<std::string>());
	node["type"] = "CapsuleCollider";
	node["endPosition"] = glm::vec3(
		((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.x,
		((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.y,
		((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.z
	);
	node["radius"] = ((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->Radius;
	return node;
}

void Twin2Engine::Core::CapsuleColliderComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Capsule Collider##").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		float v = ((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->Radius;
		ImGui::DragFloat(string("Radius##").append(id).c_str(), &v, 0.1f);

		if (v != ((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->Radius) {
			SetRadius(v);
		}

		glm::vec3 ep = ((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition;
		ImGui::DragFloat3(string("EndPosition##").append(id).c_str(), glm::value_ptr(ep), 0.1f);

		if (ep != ((CollisionSystem::CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition) {
			SetEndPosition(ep);
		}

		DrawInheritedFields();
	}
}