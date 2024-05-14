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
		//separacja obu gameobjekt�w
		SPDLOG_INFO("{} - separacja obu gameobjekt�w ({}, {}, {})", collider->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		collider->colliderComponent->GetTransform()->SetGlobalPosition(
			collider->colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
		other->colliderComponent->GetTransform()->SetGlobalPosition(
			other->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
	}
	else if (!collider->isStatic) {
		//separacja tego game objekta
		SPDLOG_INFO("{} - separacja tego game objekta ({}, {}, {})", collider->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		collider->colliderComponent->GetTransform()->SetGlobalPosition(
			collider->colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
	}
	else if (!other->isStatic) {
		//separacje drugiego gameobjekta
		SPDLOG_INFO("{} - separacje drugiego gameobjekta ({}, {}, {})", collider->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		other->colliderComponent->GetTransform()->SetGlobalPosition(
			other->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
	}
	/*else {
		//separacja obu gameobjekt�w
		SPDLOG_INFO("{} - separacja obu gameobjekt�w ({}, {}, {})", collider->colliderId, collision->separation.x,
			collision->separation.y, collision->separation.z);
		collider->colliderComponent->GetTransform()->SetGlobalPosition(
			collider->colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
		other->colliderComponent->GetTransform()->SetGlobalPosition(
			other->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
	}*/
}

ColliderComponent::ColliderComponent() : Component()
{
	boundingVolume = new BoundingVolume(new SphereColliderData());
}

void ColliderComponent::DrawInheritedFields()
{
	string id = string(std::to_string(this->GetId()));
	glm::vec3 pos = collider->shapeColliderData->LocalPosition;
	ImGui::DragFloat3(string("Offset##").append(id).c_str(), glm::value_ptr(pos), 0.1f);

	if (pos != collider->shapeColliderData->LocalPosition) {
		collider->shapeColliderData->LocalPosition = pos;
	}

	std::map<Layer, string> ls = {
		std::pair(Layer::DEFAULT, "DEFAULT"),
		std::pair(Layer::IGNORE_RAYCAST, "IGNORE_RAYCAST"),
		std::pair(Layer::IGNORE_COLLISION, "IGNORE_COLLISION"),
		std::pair(Layer::UI, "UI"),
		std::pair(Layer::LAYER_1, "LAYER_1"),
		std::pair(Layer::LAYER_2, "LAYER_2"),
		std::pair(Layer::LAYER_3, "LAYER_3"),
		std::pair(Layer::LAYER_4, "LAYER_4"),
	};

	Layer l = collider->layer;
	bool clicked = false;
	if (ImGui::BeginCombo(string("Layer##").append(id).c_str(), ls[l].c_str())) {

		for (auto& item : ls)
		{
			if (ImGui::Selectable(item.second.append("##").append(id).c_str(), l == item.first))
			{
				if (clicked) {
					continue;
				}
				l = item.first;
				clicked = true;
			}
		}
		ImGui::EndCombo();
	}

	if (clicked) {
		this->SetLayer(l);
	}

	std::unordered_map<CollisionMode, string> cms = {
		std::pair(CollisionMode::IGNORING, "IGNORING"),
		std::pair(CollisionMode::NEUTRAL, "NEUTRAL"),
		std::pair(CollisionMode::ACTIVE, "ACTIVE")
	};

	std::vector<string> lcfs = {
		"DEFAULT",
		"IGNORE_RAYCAST",
		"IGNORE_COLLISION",
		"UI",
		"LAYER_1",
		"LAYER_2",
		"LAYER_3",
		"LAYER_4",
	};

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

			if (ImGui::BeginCombo(lcfs[i].append("##").append(id).c_str(), cms[m].c_str())) {

				for (auto& item : cms)
				{
					if (ImGui::Selectable(item.second.append("##").append(id).c_str(), m == item.first))
					{
						if (clicked) {
							continue;
						}
						m = item.first;
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
}

ColliderComponent::~ColliderComponent()
{
	delete collider;
	collider = nullptr;
	delete boundingVolume;
	boundingVolume = nullptr;
}


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
	collider->colliderComponent = this;
	_onCollisionEnterId = collider->OnCollisionEnter += [](Collision* collision) -> void { OnCollisionEnter(collision); };
}

void ColliderComponent::Update()
{
	if (dirtyFlag) {
		collider->shapeColliderData->Position = GetTransform()->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);

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
	((SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius = radius;
}

void ColliderComponent::SetLocalPosition(float x, float y, float z)
{
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
	node["trigger"] = collider->isTrigger;
	node["static"] = collider->isStatic;
	node["layer"] = collider->layer;
	node["layerFilter"] = collider->layersFilter;
	node["boundingVolume"] = collider->boundingVolume != nullptr;
	if (collider->boundingVolume != nullptr) {
		node["boundingVolumeRadius"] = ((SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius;
	}
	node["position"] = glm::vec3(collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);
	return node;
}

void ColliderComponent::DrawEditor()
{
	string name = string("Collider##Component").append(std::to_string(this->GetId()));
	if (ImGui::CollapsingHeader(name.c_str())) {
		DrawInheritedFields();
	}
}