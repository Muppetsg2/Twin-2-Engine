#include "ColliderComponent.h"

using namespace Twin2EngineCore;

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
}