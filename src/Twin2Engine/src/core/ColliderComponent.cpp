#include <core/ColliderComponent.h>
#include <core/GameObject.h>
#include <CollisionManager.h>
#include <core/YamlConverters.h>

using namespace Twin2Engine::Core;

ColliderComponent::ColliderComponent() : Component()
{
	boundingVolume = new CollisionSystem::BoundingVolume(new CollisionSystem::SphereColliderData());
	//CollisionManager::Instance->RegisterCollider(this);
	//colliderComponents.push_back(this);
}

ColliderComponent::~ColliderComponent()
{
	delete collider;
	collider = nullptr;
	delete boundingVolume;
	boundingVolume = nullptr;
	//CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
	/*for (size_t i = 0; i < colliderComponents.size(); ++i) {
		if (colliderComponents[i] == this) {
			colliderComponents.erase(colliderComponents.begin() + i);
			break;
		}
	}*/
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
	CollisionSystem::CollisionManager::Instance()->UnregisterCollider(collider);
	collider->layer = layer;
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

Layer ColliderComponent::GetLayer()
{
	return collider->layer;
}

void ColliderComponent::SetLayersFilter(LayerCollisionFilter& layersFilter)
{
	collider->layersFilter = layersFilter;
}

void Twin2Engine::Core::ColliderComponent::Update()
{
	if (dirtyFlag) {
		collider->shapeColliderData->Position = GetTransform()->GetTransformMatrix() * glm::vec4(collider->shapeColliderData->LocalPosition, 1.0f);

		if (boundingVolume != nullptr) {
			boundingVolume->shapeColliderData->Position = collider->shapeColliderData->Position;
		}

		dirtyFlag = false;
		//YAML::Node Twin2Engine::Core::ColliderComponent::Serialize() const
		//{
		//	return YAML::Node();
		//}
	}
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
	((CollisionSystem::SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius = radius;
}

void ColliderComponent::SetLocalPosition(float x, float y, float z)
{
	collider->shapeColliderData->LocalPosition.x = x;
	collider->shapeColliderData->LocalPosition.y = y;
	collider->shapeColliderData->LocalPosition.z = z;

	dirtyFlag = true;
}

YAML::Node Twin2Engine::Core::ColliderComponent::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Collider";
	node["trigger"] = collider->isTrigger;
	node["static"] = collider->isStatic;
	node["layer"] = collider->layer;
	node["layerFilter"] = collider->layersFilter;
	node["boundingVolume"] = collider->boundingVolume != nullptr;
	if (collider->boundingVolume != nullptr) {
		node["boundingVolumeRadius"] = ((CollisionSystem::SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius;
	}
	node["position"] = glm::vec3(collider->shapeColliderData->LocalPosition.x, collider->shapeColliderData->LocalPosition.y, collider->shapeColliderData->LocalPosition.z);
	return node;
}

void Twin2Engine::Core::ColliderComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Collider##").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		glm::vec3 pos = collider->shapeColliderData->LocalPosition;
		ImGui::DragFloat3(string("Offset##").append(id).c_str(), glm::value_ptr(pos), 0.1f);

		if (pos != collider->shapeColliderData->LocalPosition) {
			collider->shapeColliderData->LocalPosition = pos;
		}

		/*
		//node["layer"] = collider->layer;
		std::unordered_map<Layer, string> ls = {
			std::pair(Layer::DEFAULT, "DEFAULT"),
			std::pair(Layer::IGNORE_RAYCAST, "IGNORE_RAYCAST"),
			std::pair(Layer::IGNORE_COLLISION, "IGNORE_COLLISION"),
			std::pair(Layer::UI, "UI"),
			std::pair(Layer::LAYER_1, "LAYER_1"),
			std::pair(Layer::LAYER_2, "LAYER_2"),
			std::pair(Layer::LAYER_3, "LAYER_3"),
			std::pair(Layer::LAYER_4, "LAYER_4"),
		};

		ImGui::BeginCombo(string("Layer##").append(id).c_str());
		ImGui::EndCombo();

		//node["layerFilter"] = collider->layersFilter;
		*/

		ImGui::Checkbox(string("Trigger##").append(id).c_str(), &collider->isTrigger);
		ImGui::Checkbox(string("Static##").append(id).c_str(), &collider->isStatic);

		bool v = collider->boundingVolume != nullptr;
		ImGui::Checkbox(string("Bounding Volume##").append(id).c_str(), &v);
		if (v != (collider->boundingVolume != nullptr)) {
			EnableBoundingVolume(v);
		}

		if (v) {
			ImGui::DragFloat(string("Bounding Volume Radius##").append(id).c_str(), &((CollisionSystem::SphereColliderData*)(collider->boundingVolume->shapeColliderData))->Radius, 0.1f);
		}
	}
}


/*/#include <core/ColliderComponent.h>

using namespace Twin2Engine::Core;

ColliderComponent::ColliderComponent()
{
	colliderComponents.push_back(this);
}

ColliderComponent::~ColliderComponent()
{
	for (size_t i = 0; i < colliderComponents.size(); ++i) {
		if (colliderComponents[i] == this) {
			colliderComponents.erase(colliderComponents.begin() + i);
			break;
		}
	}
}/**/
