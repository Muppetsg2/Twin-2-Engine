#include <core/HexagonalColliderComponent.h>
#include <core/GameObject.h>
#include <physic/CollisionManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

void HexagonalColliderComponent::UnDirty()
{
	ColliderComponent::UnDirty();
	HexagonalColliderData* hexData = ((HexagonalColliderData*)collider->shapeColliderData);
	glm::quat q = GetTransform()->GetGlobalRotationQuat() * glm::angleAxis(glm::radians(hexData->Rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	hexData->u = q * glm::vec3(-0.5f, 0.0f, -0.866f);
	hexData->v = q * glm::vec3(0.5f, 0.0f, -0.866f);
	hexData->w = q * glm::vec3(1.0f, 0.0f, 0.0f);

	dirtyFlag = false;
}

void HexagonalColliderComponent::SetBaseLength(float v)
{
	((HexagonalColliderData*)collider->shapeColliderData)->BaseLength = v;
}

void HexagonalColliderComponent::SetHalfHeight(float v)
{
	((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight = v;
}

// EuralAngles
void HexagonalColliderComponent::SetYRotation(float v)
{
	((HexagonalColliderData*)collider->shapeColliderData)->Rotation = v;
	dirtyFlag = true;
}

void HexagonalColliderComponent::Initialize()
{
	if (collider == nullptr) {
		collider = new GameCollider(this, ColliderShape::HEXAGONAL);
	}

	TransformChangeAction = [this](Transform* transform) {
		HexagonalColliderData* hexData = (HexagonalColliderData*)collider->shapeColliderData;
		glm::quat q = transform->GetGlobalRotationQuat() * glm::angleAxis(glm::radians(hexData->Rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		hexData->u = q * glm::vec3(-0.5f, 0.0f, -0.866f);
		hexData->v = q * glm::vec3(0.5f, 0.0f, -0.866f);
		hexData->w = q * glm::vec3(1.0f, 0.0f, 0.0f);

		collider->shapeColliderData->Position = glm::vec3(transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f));
	};

	ColliderComponent::Initialize();

	TransformChangeAction(GetTransform());
}

void HexagonalColliderComponent::Update()
{
	if (dirtyFlag) {
		UnDirty();
	}

	ColliderComponent::Update();
}

void HexagonalColliderComponent::OnEnable()
{
	ColliderComponent::OnEnable();
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
}

void HexagonalColliderComponent::OnDisable()
{
	ColliderComponent::OnDisable();
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
}

void HexagonalColliderComponent::OnDestroy()
{
	ColliderComponent::OnDestroy();
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
}

YAML::Node HexagonalColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["type"] = "HexagonalCollider";

	if (collider != nullptr) {
		node["baselength"] = ((HexagonalColliderData*)collider->shapeColliderData)->BaseLength;
		node["halfheight"] = ((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight;
		node["rotation"] = ((HexagonalColliderData*)collider->shapeColliderData)->Rotation;
	}
	else {
		node["baselength"] = 0.f;
		node["halfheight"] = 0.f;
		node["rotation"] = 0.f;
	}

	return node;
}

bool HexagonalColliderComponent::Deserialize(const YAML::Node& node)
{
	if (!node["baseLength"] || !node["halfHeight"] || !node["rotation"]) return false;

	if (collider == nullptr) {
		collider = new GameCollider(this, ColliderShape::HEXAGONAL);
	}

	if (!ColliderComponent::Deserialize(node)) return false;

	((HexagonalColliderData*)collider->shapeColliderData)->BaseLength = node["baseLength"].as<float>();
	((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight = node["halfHeight"].as<float>();
	((HexagonalColliderData*)collider->shapeColliderData)->Rotation = node["rotation"].as<float>();

	return true;
}

#if _DEBUG
void HexagonalColliderComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Hexagonal Collider##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;
		float v = ((HexagonalColliderData*)collider->shapeColliderData)->BaseLength;
		ImGui::DragFloat(string("Base Length##").append(id).c_str(), &v, 0.1f, 0.f, FLT_MAX);

		if (v != ((HexagonalColliderData*)collider->shapeColliderData)->BaseLength) {
			SetBaseLength(v);
		}

		v = ((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight;
		ImGui::DragFloat(string("Half Height##").append(id).c_str(), &v, 0.1f, 0.f, FLT_MAX);

		if (v != ((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight) {
			SetHalfHeight(v);
		}

		v = ((HexagonalColliderData*)collider->shapeColliderData)->Rotation;
		ImGui::DragFloat(string("Rotation Y##").append(id).c_str(), &v, 0.1f, -180.f, 180.f);

		if (v != ((HexagonalColliderData*)collider->shapeColliderData)->Rotation) {
			SetYRotation(v);
		}

		DrawInheritedFields();
	}
}
#endif