#include <core/BoxColliderComponent.h>
#include <CollisionManager.h>
#include <core/YamlConverters.h>

Twin2Engine::Core::BoxColliderComponent::BoxColliderComponent() : ColliderComponent()
{
	CollisionSystem::BoxColliderData* colliderData = new CollisionSystem::BoxColliderData();
	collider = new CollisionSystem::GameCollider(this, colliderData);
	CollisionSystem::CollisionManager::Instance()->RegisterCollider(collider);
}

void Twin2Engine::Core::BoxColliderComponent::SetWidth(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.x = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetLength(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.z = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetHeight(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.y = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetXRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.x = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetYRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.y = v;
}

void Twin2Engine::Core::BoxColliderComponent::SetZRotation(float v)
{
	((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.z = v;
}

YAML::Node Twin2Engine::Core::BoxColliderComponent::Serialize() const
{
	YAML::Node node = ColliderComponent::Serialize();
	node["subTypes"].push_back(node["type"].as<std::string>());
	node["type"] = "BoxCollider";
	node["width"] = ((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.x;
	node["length"] = ((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.z;
	node["height"] = ((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->HalfDimensions.y;
	node["rotation"] = glm::vec3(
		((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.x,
		((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.y,
		((CollisionSystem::BoxColliderData*)collider->shapeColliderData)->Rotation.z
	);
	return node;
}
