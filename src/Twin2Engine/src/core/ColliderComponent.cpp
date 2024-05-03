#include <core/ColliderComponent.h>
#include <core/GameObject.h>
#include <physics/CollisionManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::PhysicsEngine;
using namespace Twin2Engine::Tools;

void ColliderComponent::OnCollisionEnter(Collision* collision)
{
	GameCollider* collider = (GameCollider*)collision->collider;
	GameCollider* other = (GameCollider*)collision->otherCollider;

	if (!collider->isStatic && !other->isStatic) {
		//separacja obu gameobjektów
		SPDLOG_INFO("{} - separacja obu gameobjektów ({}, {}, {})", collider->colliderId, collision->separation.x,
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
		//separacja obu gameobjektów
		SPDLOG_INFO("{} - separacja obu gameobjektów ({}, {}, {})", collider->colliderId, collision->separation.x,
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