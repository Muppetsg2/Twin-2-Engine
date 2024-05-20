#include <core/CapsuleColliderComponent.h>
#include <physic/CollisionManager.h>
#include <core/GameObject.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Physic;
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
	node["type"] = "CapsuleCollider";
	node["endPosition"] = ((CapsuleColliderData*)collider->shapeColliderData)->EndLocalPosition;
	node["radius"] = ((CapsuleColliderData*)collider->shapeColliderData)->Radius;
	return node;
}

bool CapsuleColliderComponent::Deserialize(const YAML::Node& node)
{
	if (!node["endPosition"] || !node["radius"] || !ColliderComponent::Deserialize(node)) return false;

	((CapsuleColliderData*)collider->shapeColliderData)->EndPosition = node["endPosition"].as<glm::vec3>();
	((CapsuleColliderData*)collider->shapeColliderData)->Radius = node["radius"].as<float>();

	return true;
}

void Twin2Engine::Core::CapsuleColliderComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Capsule Collider##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		Component::DrawInheritedFields();

		float v = ((CapsuleColliderData*)collider->shapeColliderData)->Radius;
		ImGui::DragFloat(string("Radius##").append(id).c_str(), &v, 0.1f);

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
