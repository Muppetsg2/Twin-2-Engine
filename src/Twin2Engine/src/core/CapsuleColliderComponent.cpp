#include <core/CapsuleColliderComponent.h>
#include <physic/CollisionManager.h>
#include <core/GameObject.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Core;

void CapsuleColliderComponent::SetEndPosition(float x, float y, float z)
{
	((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.x = x;
	((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.y = y;
	((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition.z = z;
	dirtyFlag = true;
}

void Twin2Engine::Core::CapsuleColliderComponent::SetEndPosition(const glm::vec3& pos)
{
	SetEndPosition(pos.x, pos.y, pos.z);
}

void CapsuleColliderComponent::SetRadius(float radius)
{
	((CapsuleColliderData*)collider->shapeColliderData)->Radius = radius;
}

void CapsuleColliderComponent::Initialize()
{
	if (collider == nullptr) {
		collider = new GameCollider(this, ColliderShape::CAPSULE);
	}

	TransformChangeAction = [this](Transform* transform) {
		glm::mat4 TransformMatrix = transform->GetTransformMatrix();
		CapsuleColliderData* capsuleData = (CapsuleColliderData*)collider->shapeColliderData;
		capsuleData->EndPosition = TransformMatrix * glm::vec4(capsuleData->EndLocalPosition, 1.0f);
		capsuleData->Position = TransformMatrix * glm::vec4(capsuleData->LocalPosition, 1.0f);

		/*
		if (collider->hasBounding) {
			collider->boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
		*/
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
	node["type"] = "CapsuleCollider";

	if (collider != nullptr) {
		node["endPosition"] = ((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition;
		node["radius"] = ((CapsuleColliderData*)collider->shapeColliderData)->Radius;
	}
	else {
		node["endPosition"] = glm::vec3(0.f);
		node["radius"] = 0.f;
	}

	return node;
}

bool CapsuleColliderComponent::Deserialize(const YAML::Node& node)
{
	if (!node["endPosition"] || !node["radius"]) return false;

	if (collider == nullptr) {
		collider = new GameCollider(this, ColliderShape::CAPSULE);
	}

	if (!ColliderComponent::Deserialize(node)) return false;

	((CapsuleColliderData*)collider->shapeColliderData)->EndPosition = node["endPosition"].as<glm::vec3>();
	((CapsuleColliderData*)collider->shapeColliderData)->Radius = node["radius"].as<float>();

	return true;
}

#if _DEBUG
void CapsuleColliderComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Capsule Collider##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;

		float v = ((CapsuleColliderData*)collider->shapeColliderData)->Radius;
		ImGui::DragFloat(string("Radius##").append(id).c_str(), &v, 0.1f, 0.f, FLT_MAX);

		if (v != ((CapsuleColliderData*)collider->shapeColliderData)->Radius) {
			SetRadius(v);
		}

		glm::vec3 ep = ((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition;
		ImGui::DragFloat3(string("EndPosition##").append(id).c_str(), glm::value_ptr(ep), 0.1f);

		if (ep != ((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition) {
			SetEndPosition(ep);
		}

		DrawInheritedFields();
	}
}
#endif