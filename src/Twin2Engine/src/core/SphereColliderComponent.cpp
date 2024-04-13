#include <core/SphereColliderComponent.h>
#include <CollisionManager.h>
#include <core/YamlConverters.h>

Twin2Engine::Core::SphereColliderComponent::SphereColliderComponent() : ColliderComponent()
{
	CollisionSystem::SphereColliderData* colliderData = new CollisionSystem::SphereColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::SphereColliderComponent::SetRadius(float radius)
{
	((CollisionSystem::SphereColliderData*)collider->shapeColliderData)->Radius = radius;
}

YAML::Node Twin2Engine::Core::SphereColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"]);
	node["type"] = "SphereCollider";
	node["radius"] = ((CollisionSystem::SphereColliderData*)collider->shapeColliderData)->Radius;
	return node;
}
