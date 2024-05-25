#include <core/ColliderComponent.h>
#include <core/GameObject.h>
#include <physic/CollisionManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Tools;

void ColliderComponent::OnCollisionEnter(Collision* collision)
{
	GameCollider* collider = (GameCollider*)collision->collider;
	GameCollider* other = (GameCollider*)collision->otherCollider;

	if (!collider->isStatic && !other->isStatic) {
		//separacja obu gameobjektow
		SPDLOG_INFO("{} - separacja obu gameobjektow ({}, {}, {})", collider->colliderComponent->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		collider->colliderComponent->GetTransform()->SetGlobalPosition(
			collider->colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
		other->colliderComponent->GetTransform()->SetGlobalPosition(
			other->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
	}
	else if (!collider->isStatic) {
		//separacja tego game objekta
		SPDLOG_INFO("{} - separacja tego game objekta ({}, {}, {})", collider->colliderComponent->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		collider->colliderComponent->GetTransform()->SetGlobalPosition(
			collider->colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
	}
	else if (!other->isStatic) {
		//separacje drugiego gameobjekta
		SPDLOG_INFO("{} - separacje drugiego gameobjekta ({}, {}, {})", collider->colliderComponent->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		other->colliderComponent->GetTransform()->SetGlobalPosition(
			other->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
	}
	/*else {
		//separacja obu gameobjektow
		SPDLOG_INFO("{} - separacja obu gameobjektow ({}, {}, {})", collider->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		collider->colliderComponent->GetTransform()->SetGlobalPosition(
			collider->colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
		other->colliderComponent->GetTransform()->SetGlobalPosition(
			other->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
	}*/
}

#if _DEBUG
bool ColliderComponent::DrawInheritedFields()
{
	string id = string(std::to_string(this->GetId()));
	glm::vec3 pos = collider->shapeColliderData->LocalPosition;
	ImGui::DragFloat3(string("Offset##").append(id).c_str(), glm::value_ptr(pos), 0.1f);

	int colId = colliderId;
	ImGui::InputInt(string("ColliderId##").append(id).c_str(), &colId);

	if (pos != collider->shapeColliderData->LocalPosition) {
		collider->shapeColliderData->LocalPosition = pos;
	}

	Layer l = collider->layer;
	bool clicked = false;
	if (ImGui::BeginCombo(string("Layer##").append(id).c_str(), to_string(l).c_str())) {

		for (size_t i = 0; i < size<Layer>(); ++i)
		{
			Layer lc = (Layer)(uint8_t)(i == 0 ? 0 : powf(2.f, (float)(i - 1)));
			if (ImGui::Selectable(to_string(lc).append("##").append(id).c_str(), l == lc))
			{
				if (clicked) {
					continue;
				}
				l = lc;
				clicked = true;
			}
		}
		ImGui::EndCombo();
	}

	if (clicked) {
		this->SetLayer(l);
	}

	std::vector<string> lcfs = std::vector<string>();

	for (size_t i = 0; i < size<Layer>(); ++i) {
		lcfs.push_back(to_string((Layer)(uint8_t)(i == 0 ? 0 : powf(2.f, (float)(i - 1)))));
	}

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(string("Layers Filters##").append(id).c_str(), node_flag);

	if (node_open) {

		LayerCollisionFilter fil = collider->layersFilter;
		bool changed = false;
		for (int i = 0; i < 8; ++i) {

			CollisionMode m = fil.DEFAULT;
			bool clicked = false;

			switch (i)
			{
				case 0: {
					m = fil.DEFAULT;
					break;
				}
				case 1: {
					m = fil.IGNORE_RAYCAST;
					break;
				}
				case 2: {
					m = fil.IGNORE_COLLISION;
					break;
				}
				case 3: {
					m = fil.UI;
					break;
				}
				case 4: {
					m = fil.LAYER_1;
					break;
				}
				case 5: {
					m = fil.LAYER_2;
					break;
				}
				case 6: {
					m = fil.LAYER_3;
					break;
				}
				case 7: {
					m = fil.LAYER_4;
					break;
				}
				default: {
					m = fil.DEFAULT;
					break;
				}
			}

			if (ImGui::BeginCombo(lcfs[i].append("##").append(id).c_str(), to_string((CollisionMode)m).c_str())) {

				for (size_t z = 0; z < size<CollisionMode>(); ++z)
				{
					if (ImGui::Selectable(to_string((CollisionMode)z).append("##").append(id).c_str(), m == (CollisionMode)z))
					{
						if (clicked) {
							continue;
						}
						m = (CollisionMode)z;
						clicked = true;
					}
				}
				ImGui::EndCombo();
			}

			if (clicked) {
				changed = true;
				switch (i)
				{
					case 0: {
						fil.DEFAULT = m;
						break;
					}
					case 1: {
						fil.IGNORE_RAYCAST = m;
						break;
					}
					case 2: {
						fil.IGNORE_COLLISION = m;
						break;
					}
					case 3: {
						fil.UI = m;
						break;
					}
					case 4: {
						fil.LAYER_1 = m;
						break;
					}
					case 5: {
						fil.LAYER_2 = m;
						break;
					}
					case 6: {
						fil.LAYER_3 = m;
						break;
					}
					case 7: {
						fil.LAYER_4 = m;
						break;
					}
					default: {
						fil.DEFAULT = m;
						break;
					}
				}
			}
		}

		if (changed) {
			this->SetLayersFilter(fil);
		}

		ImGui::TreePop();
	}

	ImGui::Checkbox(string("Trigger##").append(id).c_str(), &collider->isTrigger);
	ImGui::Checkbox(string("Static##").append(id).c_str(), &collider->isStatic);

	bool v = collider->boundingVolume != nullptr;
	ImGui::Checkbox(string("Bounding Volume##").append(id).c_str(), &v);
	if (v != (collider->boundingVolume != nullptr)) {
		EnableBoundingVolume(v);
	}

	if (v) {
		ImGui::DragFloat(string("Bounding Volume Radius##").append(id).c_str(), &((SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius, 0.1f);
	}

	return false;
}
#endif

void ColliderComponent::SetTrigger(bool v)
{
	collider->isTrigger = v;
}

bool ColliderComponent::IsTrigger()
{
	return collider->isTrigger;
}

void ColliderComponent::SetStatic(bool v)
{
	collider->isStatic = v;
}

bool ColliderComponent::IsStatic()
{
	return collider->isStatic;
}

void ColliderComponent::SetLayer(Layer layer)
{
	CollisionManager::Instance()->UnregisterCollider(collider);
	collider->layer = layer;
	CollisionManager::Instance()->RegisterCollider(collider);
}

Layer ColliderComponent::GetLayer()
{
	return collider->layer;
}

void ColliderComponent::SetLayersFilter(LayerCollisionFilter& layersFilter)
{
	collider->layersFilter = layersFilter;
}

void ColliderComponent::Initialize()
{
	if (boundingVolume == nullptr) {
		boundingVolume = new BoundingVolume(new SphereColliderData());
	}
	collider->colliderComponent = this;
	_onCollisionEnterId = collider->OnCollisionEnter += [](Collision* collision) -> void { OnCollisionEnter(collision); };
}

void ColliderComponent::Update()
{
	if (dirtyFlag) {
		//auto Pos = GetTransform()->GetGlobalPosition();
		//SPDLOG_INFO("{}.  Pos: \t\t{}\t{}\t{}", colliderId, Pos.x, Pos.y, Pos.z);
		//SPDLOG_INFO("{}.  ColPos: \t{}\t{}\t{}\t\t\t\tLocPos: \t{}\t{}\t{}", colliderId, collider->shapeColliderData->Position.x, collider->shapeColliderData->Position.y, collider->shapeColliderData->Position.z, collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);
		//auto TM = GetTransform()->GetTransformMatrix();
		//auto v4 = GetTransform()->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);
		collider->shapeColliderData->Position = glm::vec3(GetTransform()->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f));
		//SPDLOG_INFO("{}.  NewPos: \t{}\t{}\t{}\t\t\t\tLocPos: \t{}\t{}\t{}", colliderId, collider->shapeColliderData->Position.x, collider->shapeColliderData->Position.y, collider->shapeColliderData->Position.z, collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}

		dirtyFlag = false;
	}
}

void ColliderComponent::OnEnable()
{
	collider->enabled = true;
}

void ColliderComponent::OnDisable()
{
	collider->enabled = false;
}

void ColliderComponent::OnDestroy()
{
	collider->OnCollisionEnter -= _onCollisionEnterId;
	delete collider;
	collider = nullptr;
	delete boundingVolume;
	boundingVolume = nullptr;
}

void ColliderComponent::EnableBoundingVolume(bool v)
{
	if (v) {
		collider->boundingVolume = boundingVolume;
	}
	else {
		collider->boundingVolume = nullptr;
	}
}

void ColliderComponent::SetBoundingVolumeRadius(float radius)
{
	((SphereColliderData*)(boundingVolume->shapeColliderData))->Radius = radius;
}

void ColliderComponent::SetLocalPosition(float x, float y, float z)
{
	//auto v = this;
	collider->shapeColliderData->LocalPosition.x = x;
	collider->shapeColliderData->LocalPosition.y = y;
	collider->shapeColliderData->LocalPosition.z = z;

	dirtyFlag = true;
}

EventHandler<Collision*>& ColliderComponent::GetOnTriggerEnterEvent() const
{
	return collider->OnTriggerEnter;
}

EventHandler<GameCollider*>& ColliderComponent::GetOnTriggerExitEvent() const
{
	return collider->OnTriggerExit;
}

EventHandler<Collision*>& ColliderComponent::GetOnCollisionEnterEvent() const
{
	return collider->OnCollisionEnter;
}

EventHandler<GameCollider*>& ColliderComponent::GetOnCollisionExitEvent() const
{
	return collider->OnCollisionExit;
}

YAML::Node ColliderComponent::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Collider";
	node["colliderId"] = colliderId;
	node["trigger"] = collider->isTrigger;
	node["static"] = collider->isStatic;
	node["layer"] = collider->layer;
	node["layersFilter"] = collider->layersFilter;
	if (collider->boundingVolume != nullptr) {
		node["boundingVolumeRadius"] = ((SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius;
	}
	node["position"] = collider->shapeColliderData->LocalPosition;
	return node;
}

bool ColliderComponent::Deserialize(const YAML::Node& node)
{
	if (!node["colliderId"] || !node["trigger"] || !node["static"] || !node["layer"] || 
		!node["layersFilter"] ||
		!node["position"] || !Component::Deserialize(node)) return false;

	colliderId = node["colliderId"].as<size_t>();
	collider->isTrigger = node["trigger"].as<bool>();
	collider->isStatic = node["static"].as<bool>();
	collider->layer = node["layer"].as<Layer>();
	collider->layersFilter = node["layersFilter"].as<LayerCollisionFilter>();
	if (node["boundingVolumeRadius"]) {
		boundingVolume = new BoundingVolume(new SphereColliderData());
		collider->boundingVolume = boundingVolume;
		((SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius = node["boundingVolumeRadius"].as<float>();
	}
	collider->shapeColliderData->LocalPosition = node["position"].as<glm::vec3>();

	return true;
}

#if _DEBUG
void ColliderComponent::DrawEditor()
{
	string name = string("Collider##Component").append(std::to_string(this->GetId()));
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;
		DrawInheritedFields();
	}
}
#endif