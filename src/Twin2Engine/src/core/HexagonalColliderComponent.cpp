#include <core/HexagonalColliderComponent.h>
#include "core/GameObject.h"
#include <physic/CollisionManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

HexagonalColliderComponent::HexagonalColliderComponent() : ColliderComponent()
{
	collider = new GameCollider(this, new HexagonalColliderData());
}

void HexagonalColliderComponent::SetBaseLength(float v)
{
	((HexagonalColliderData*)collider->shapeColliderData)->BaseLength = v;
}

void HexagonalColliderComponent::SetHalfHeight(float v)
{
	((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight = v;
}

void HexagonalColliderComponent::SetYRotation(float v)
{
	((HexagonalColliderData*)collider->shapeColliderData)->Rotation = v;
	dirtyFlag = true;
}

void HexagonalColliderComponent::Initialize()
{
	collider->colliderComponent = this;
	TransformChangeAction = [this](Transform* transform) {
		HexagonalColliderData* hexData = ((HexagonalColliderData*)collider->shapeColliderData);
		glm::quat q = transform->GetGlobalRotationQuat() * glm::angleAxis(hexData->Rotation, glm::vec3(0.0f, 1.0f, 0.0f));
		hexData->u = q * glm::vec3(-0.5f, 0.0f, -0.866f);
		hexData->v = q * glm::vec3(0.5f, 0.0f, -0.866f);
		hexData->w = q * glm::vec3(1.0f, 0.0f, 0.0f);

		//hexData->HalfHeight = transform->GetGlobalScale().y * hexData->HalfHeight;

		auto Pos = transform->GetGlobalPosition();
		//SPDLOG_INFO("{}.  Pos: \t\t{}\t{}\t{}", colliderId, Pos.x, Pos.y, Pos.z);
		//SPDLOG_INFO("{}.  ColPos: \t{}\t{}\t{}\t\t\t\tLocPos: \t{}\t{}\t{}", colliderId, collider->shapeColliderData->Position.x, collider->shapeColliderData->Position.y, collider->shapeColliderData->Position.z, collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);
		//collider->shapeColliderData->Position = glm::vec3(transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f));
		//auto TM = transform->GetTransformMatrix();
		collider->shapeColliderData->Position = glm::vec3(transform->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f));
		//SPDLOG_INFO("{}.  NewPos: \t{}\t{}\t{}\t\t\t\tLocPos: \t{}\t{}\t{}", colliderId, collider->shapeColliderData->Position.x, collider->shapeColliderData->Position.y, collider->shapeColliderData->Position.z, collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}
	};

	//TransformChangeAction(GetTransform());
}

void HexagonalColliderComponent::OnEnable()
{
	TransformChangeActionId = GetTransform()->OnEventTransformChanged += TransformChangeAction;
	CollisionManager* cm = CollisionManager::Instance();
	cm->RegisterCollider(collider);
}

void HexagonalColliderComponent::OnDisable()
{
	GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

void HexagonalColliderComponent::OnDestroy()
{
	//GetTransform()->OnEventTransformChanged -= TransformChangeActionId;
	CollisionManager::Instance()->UnregisterCollider(collider);
}

void HexagonalColliderComponent::Update()
{
	ColliderComponent::Update();

	if (dirtyFlag) {
		HexagonalColliderData * hexData = ((HexagonalColliderData*)collider->shapeColliderData);
		glm::quat q = GetTransform()->GetGlobalRotationQuat() * glm::angleAxis(hexData->Rotation, glm::vec3(0.0f, 1.0f, 0.0f));
		hexData->u = q * glm::vec3(-0.5f, 0.0f, -0.866f);
		hexData->v = q * glm::vec3(0.5f, 0.0f, -0.866f);
		hexData->w = q * glm::vec3(1.0f, 0.0f, 0.0f);

		//hexData->HalfHeight = GetTransform()->GetGlobalScale().y * hexData->HalfHeight;

		dirtyFlag = false;
	}
}

YAML::Node HexagonalColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["type"] = "HexagonalCollider";
	node["baselength"] = ((HexagonalColliderData*)collider->shapeColliderData)->BaseLength;
	node["halfheight"] = ((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight;
	node["rotation"] = ((HexagonalColliderData*)collider->shapeColliderData)->Rotation;
	return node;
}

bool HexagonalColliderComponent::Deserialize(const YAML::Node& node)
{
	if (!node["baseLength"] || !node["halfHeight"] || !node["rotation"] ||
		!ColliderComponent::Deserialize(node)) return false;

	((HexagonalColliderData*)collider->shapeColliderData)->BaseLength = node["baseLength"].as<float>();
	((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight = node["halfHeight"].as<float>();
	((HexagonalColliderData*)collider->shapeColliderData)->Rotation = node["rotation"].as<float>();

	return true;
}

void HexagonalColliderComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Hexagonal Collider##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		float v = ((HexagonalColliderData*)collider->shapeColliderData)->BaseLength;
		ImGui::DragFloat(string("Base Length##").append(id).c_str(), &v, 0.1f);

		if (v != ((HexagonalColliderData*)collider->shapeColliderData)->BaseLength) {
			SetBaseLength(v);
		}

		v = ((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight;
		ImGui::DragFloat(string("Half Height##").append(id).c_str(), &v, 0.1f);

		if (v != ((HexagonalColliderData*)collider->shapeColliderData)->HalfHeight) {
			SetHalfHeight(v);
		}

		v = ((HexagonalColliderData*)collider->shapeColliderData)->Rotation;
		ImGui::DragFloat(string("Rotation Y##").append(id).c_str(), &v, 0.1f);

		if (v != ((HexagonalColliderData*)collider->shapeColliderData)->Rotation) {
			SetYRotation(v);
		}

		DrawInheritedFields();
	}
}