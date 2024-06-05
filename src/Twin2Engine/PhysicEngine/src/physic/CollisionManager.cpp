#include <physic/CollisionManager.h>

using namespace Twin2Engine::Physic;

CollisionManager* CollisionManager::instance = nullptr;

CollisionManager::CollisionManager() {
	DefaultLayer.clear();
	IgnoreRaycastLayer.clear();
	IgnoreCollisionLayer.clear();
	UiLayer.clear();
	Layer1.clear();
	Layer2.clear();
	Layer3.clear();
	Layer4.clear();
}

CollisionManager::~CollisionManager() {
	DefaultLayer.clear();
	IgnoreRaycastLayer.clear();
	IgnoreCollisionLayer.clear();
	UiLayer.clear();
	Layer1.clear();
	Layer2.clear();
	Layer3.clear();
	Layer4.clear();
}

CollisionManager* CollisionManager::Instance() {
	if (instance == nullptr) {
		instance = new CollisionManager();
	}

	return instance;
}

void CollisionManager::UnloadAll() {
	delete instance;
	instance = nullptr;
}

void CollisionManager::RegisterCollider(GameCollider* collider) {
	switch (collider->layer) {
	case Layer::DEFAULT:
		DefaultLayer.insert(collider);
		break;

	case Layer::IGNORE_RAYCAST:
		IgnoreRaycastLayer.insert(collider);
		break;

	case Layer::IGNORE_COLLISION:
		IgnoreCollisionLayer.insert(collider);
		break;

	case Layer::UI:
		UiLayer.insert(collider);
		break;

	case Layer::LAYER_1:
		Layer1.insert(collider);
		break;

	case Layer::LAYER_2:
		Layer2.insert(collider);
		break;

	case Layer::LAYER_3:
		Layer3.insert(collider);
		break;

	case Layer::LAYER_4:
		Layer4.insert(collider);
		break;
	}
}

void CollisionManager::UnregisterCollider(GameCollider* collider) {
	switch (collider->layer) {
	case Layer::DEFAULT:
		DefaultLayer.erase(collider);
		break;

	case Layer::IGNORE_RAYCAST:
		IgnoreRaycastLayer.erase(collider);
		break;

	case Layer::IGNORE_COLLISION:
		IgnoreCollisionLayer.erase(collider);
		break;

	case Layer::UI:
		UiLayer.erase(collider);
		break;

	case Layer::LAYER_1:
		Layer1.erase(collider);
		break;

	case Layer::LAYER_2:
		Layer2.erase(collider);
		break;

	case Layer::LAYER_3:
		Layer3.erase(collider);
		break;

	case Layer::LAYER_4:
		Layer4.erase(collider);
		break;
	}
}

void CollisionManager::PerformCollisions() {
	for (auto collider : IgnoreCollisionLayer) {
		//kolizja z DEFAULT LAYER
		if (collider->layersFilter.DEFAULT == CollisionMode::ACTIVE) {
			for (auto other : DefaultLayer) {
				if (other->layersFilter.IGNORE_COLLISION >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.DEFAULT == CollisionMode::NEUTRAL) {
			for (auto other : DefaultLayer) {
				if (other->layersFilter.IGNORE_COLLISION == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.IGNORE_RAYCAST == CollisionMode::ACTIVE) {
			for (auto other : IgnoreRaycastLayer) {
				if (other->layersFilter.IGNORE_RAYCAST >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.IGNORE_RAYCAST == CollisionMode::NEUTRAL) {
			for (auto other : IgnoreRaycastLayer) {
				if (other->layersFilter.IGNORE_RAYCAST == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
			for (auto other : Layer1) {
				if (other->layersFilter.LAYER_1 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_1 == CollisionMode::NEUTRAL) {
			for (auto other : Layer1) {
				if (other->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_2 == CollisionMode::NEUTRAL) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_3 == CollisionMode::NEUTRAL) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_4 == CollisionMode::NEUTRAL) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}
	}

	auto Begining = DefaultLayer.begin();
	auto Current = Begining;
	for (auto collider : DefaultLayer) {
		if (collider->layersFilter.DEFAULT == CollisionMode::ACTIVE) {
			Current = Begining;
			++Current;
			while (Current != DefaultLayer.end()) {
				if ((*Current)->layersFilter.DEFAULT >= CollisionMode::NEUTRAL) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}
		else if (collider->layersFilter.DEFAULT == CollisionMode::NEUTRAL) {
			Current = Begining;
			++Current;
			while (Current != DefaultLayer.end()) {
				if ((*Current)->layersFilter.DEFAULT == CollisionMode::ACTIVE) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}

		//kolizja z IgnoreRaycastLayer
		if (collider->layersFilter.IGNORE_RAYCAST == CollisionMode::ACTIVE) {
			for (auto other : IgnoreRaycastLayer) {
				if (other->layersFilter.IGNORE_RAYCAST >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.IGNORE_RAYCAST == CollisionMode::NEUTRAL) {
			for (auto other : IgnoreRaycastLayer) {
				if (other->layersFilter.IGNORE_RAYCAST == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
			for (auto other : Layer1) {
				if (other->layersFilter.LAYER_1 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_1 == CollisionMode::NEUTRAL) {
			for (auto other : Layer1) {
				if (other->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_2 == CollisionMode::NEUTRAL) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_3 == CollisionMode::NEUTRAL) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_4 == CollisionMode::NEUTRAL) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}
	}

	Begining = IgnoreRaycastLayer.begin();
	for (auto collider : IgnoreRaycastLayer) {
		if (collider->layersFilter.IGNORE_RAYCAST == CollisionMode::ACTIVE) {
			Current = Begining;
			++Current;
			while (Current != IgnoreRaycastLayer.end()) {
				if ((*Current)->layersFilter.IGNORE_RAYCAST >= CollisionMode::NEUTRAL) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}
		else if (collider->layersFilter.IGNORE_RAYCAST == CollisionMode::NEUTRAL) {
			Current = Begining;
			++Current;
			while (Current != IgnoreRaycastLayer.end()) {
				if ((*Current)->layersFilter.IGNORE_RAYCAST == CollisionMode::ACTIVE) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}

		if (collider->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
			for (auto other : Layer1) {
				if (other->layersFilter.LAYER_1 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_1 == CollisionMode::NEUTRAL) {
			for (auto other : Layer1) {
				if (other->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_2 == CollisionMode::NEUTRAL) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_3 == CollisionMode::NEUTRAL) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_4 == CollisionMode::NEUTRAL) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}
	}

	Begining = Layer1.begin();
	for (auto collider : Layer1) {
		if (collider->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
			Current = Begining;
			++Current;
			while (Current != Layer1.end()) {
				if ((*Current)->layersFilter.LAYER_1 >= CollisionMode::NEUTRAL) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}
		else if (collider->layersFilter.LAYER_1 == CollisionMode::NEUTRAL) {
			Current = Begining;
			++Current;
			while (Current != Layer1.end()) {
				if ((*Current)->layersFilter.LAYER_1 == CollisionMode::ACTIVE) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}

		if (collider->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_2 == CollisionMode::NEUTRAL) {
			for (auto other : Layer2) {
				if (other->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_3 == CollisionMode::NEUTRAL) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_4 == CollisionMode::NEUTRAL) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}
	}

	Begining = Layer2.begin();
	for (auto collider : Layer2) {
		if (collider->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
			Current = Begining;
			++Current;
			while (Current != Layer2.end()) {
				if ((*Current)->layersFilter.LAYER_2 >= CollisionMode::NEUTRAL) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}
		else if (collider->layersFilter.LAYER_2 == CollisionMode::NEUTRAL) {
			Current = Begining;
			++Current;
			while (Current != Layer2.end()) {
				if ((*Current)->layersFilter.LAYER_2 == CollisionMode::ACTIVE) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}

		if (collider->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_3 == CollisionMode::NEUTRAL) {
			for (auto other : Layer3) {
				if (other->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}

		if (collider->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_4 == CollisionMode::NEUTRAL) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}
	}

	Begining = Layer3.begin();
	for (auto collider : Layer3) {
		if (collider->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
			Current = Begining;
			++Current;
			while (Current != Layer3.end()) {
				if ((*Current)->layersFilter.LAYER_3 >= CollisionMode::NEUTRAL) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}
		else if (collider->layersFilter.LAYER_3 == CollisionMode::NEUTRAL) {
			Current = Begining;
			++Current;
			while (Current != Layer3.end()) {
				if ((*Current)->layersFilter.LAYER_3 == CollisionMode::ACTIVE) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}

		if (collider->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 >= CollisionMode::NEUTRAL) {
					delete collider->collide(other);
				}
			}
		}
		else if (collider->layersFilter.LAYER_4 == CollisionMode::NEUTRAL) {
			for (auto other : Layer4) {
				if (other->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
					delete collider->collide(other);
				}
			}
		}
	}

	Begining = Layer4.begin();
	for (auto collider : Layer4) {
		if (collider->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
			Current = Begining;
			++Current;
			while (Current != Layer4.end()) {
				if ((*Current)->layersFilter.LAYER_4 >= CollisionMode::NEUTRAL) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}
		else if (collider->layersFilter.LAYER_4 == CollisionMode::NEUTRAL) {
			Current = Begining;
			++Current;
			while (Current != Layer4.end()) {
				if ((*Current)->layersFilter.LAYER_4 == CollisionMode::ACTIVE) {
					delete collider->collide((*Current));
				}

				++Current;
			}
			++Begining;
		}
	}
}


bool CollisionManager::Raycast(Ray& ray, RaycastHit& raycastHit)
{
	//Collision* collision;
	float minDistance = -1.0f;
	//float tDistance;
	float sqrDistance;
	float projection;
	glm::vec3 tRelPos;
	RaycastHit temp;

	for (auto collider : DefaultLayer) {
		//collision = collider->rayCollision(ray);
		if (collider->rayCollision(ray, temp)) {
			tRelPos = temp.position - ray.Origin;
			sqrDistance = glm::dot(tRelPos, tRelPos);
			projection = glm::dot(tRelPos, ray.Direction);
			if (minDistance < 0 && projection >= 0.0f) {
				minDistance = sqrDistance;
				raycastHit.collider = temp.collider;
				raycastHit.position = temp.position;
			}
			else {
				if (sqrDistance < minDistance && projection >= 0.0f) {
					minDistance = sqrDistance;
					raycastHit.collider = temp.collider;
					raycastHit.position = temp.position;
				}
			}
		}
	}
	for (auto collider : IgnoreCollisionLayer) {
		//collision = collider->rayCollision(ray);
		if (collider->rayCollision(ray, temp)) {
			tRelPos = temp.position - ray.Origin;
			sqrDistance = glm::dot(tRelPos, tRelPos);
			projection = glm::dot(tRelPos, ray.Direction);
			if (minDistance < 0 && projection >= 0.0f) {
				minDistance = sqrDistance;
				raycastHit.collider = temp.collider;
				raycastHit.position = temp.position;
			}
			else {
				if (sqrDistance < minDistance && projection >= 0.0f) {
					minDistance = sqrDistance;
					raycastHit.collider = temp.collider;
					raycastHit.position = temp.position;
				}
			}
		}
	}
	for (auto collider : Layer1) {
		//collision = collider->rayCollision(ray);
		if (collider->rayCollision(ray, temp)) {
			tRelPos = temp.position - ray.Origin;
			sqrDistance = glm::dot(tRelPos, tRelPos);
			projection = glm::dot(tRelPos, ray.Direction);
			if (minDistance < 0 && projection >= 0.0f) {
				minDistance = sqrDistance;
				raycastHit.collider = temp.collider;
				raycastHit.position = temp.position;
			}
			else {
				if (sqrDistance < minDistance && projection >= 0.0f) {
					minDistance = sqrDistance;
					raycastHit.collider = temp.collider;
					raycastHit.position = temp.position;
				}
			}
		}
	}
	for (auto collider : Layer2) {
		if (collider->rayCollision(ray, temp)) {
			tRelPos = temp.position - ray.Origin;
			sqrDistance = glm::dot(tRelPos, tRelPos);
			projection = glm::dot(tRelPos, ray.Direction);
			if (minDistance < 0 && projection >= 0.0f) {
				minDistance = sqrDistance;
				raycastHit.collider = temp.collider;
				raycastHit.position = temp.position;
			}
			else {
				if (sqrDistance < minDistance && projection >= 0.0f) {
					minDistance = sqrDistance;
					raycastHit.collider = temp.collider;
					raycastHit.position = temp.position;
				}
			}
		}
	}
	for (auto collider : Layer3) {
		if (collider->rayCollision(ray, temp)) {
			tRelPos = temp.position - ray.Origin;
			sqrDistance = glm::dot(tRelPos, tRelPos);
			projection = glm::dot(tRelPos, ray.Direction);
			if (minDistance < 0 && projection >= 0.0f) {
				minDistance = sqrDistance;
				raycastHit.collider = temp.collider;
				raycastHit.position = temp.position;
			}
			else {
				if (sqrDistance < minDistance && projection >= 0.0f) {
					minDistance = sqrDistance;
					raycastHit.collider = temp.collider;
					raycastHit.position = temp.position;
				}
			}
		}
	}
	for (auto collider : Layer4) {
		if (collider->rayCollision(ray, temp)) {
			tRelPos = temp.position - ray.Origin;
			sqrDistance = glm::dot(tRelPos, tRelPos);
			projection = glm::dot(tRelPos, ray.Direction);
			if (minDistance < 0 && projection >= 0.0f) {
				minDistance = sqrDistance;
				raycastHit.collider = temp.collider;
				raycastHit.position = temp.position;
			}
			else {
				if (sqrDistance < minDistance && projection >= 0.0f) {
					minDistance = sqrDistance;
					raycastHit.collider = temp.collider;
					raycastHit.position = temp.position;
				}
			}
		}
	}

	//delete collision;

	return raycastHit.collider != nullptr;
}

void Twin2Engine::Physic::CollisionManager::OverlapSphere(glm::vec3 spherePosition, float radius, std::vector<Twin2Engine::Core::ColliderComponent*>& collidingWithSphere)
{
	SphereColliderData* sphereShape = new SphereColliderData();
	sphereShape->LocalPosition = spherePosition;
	sphereShape->Position = spherePosition;
	sphereShape->Radius = radius;
	Collider* collider = new GameCollider(nullptr, sphereShape);

	Twin2Engine::Physic::Collision* collision;

	for (auto other : DefaultLayer) {
		collision = Twin2Engine::Physic::Collider::testCollision(collider, other, false);
		if (collision != nullptr) {
			collidingWithSphere.push_back(((Twin2Engine::Physic::GameCollider*)collision->otherCollider)->colliderComponent);
			delete collision;
		}
	}

	for (auto other : IgnoreRaycastLayer) {
		collision = Twin2Engine::Physic::Collider::testCollision(collider, other, false);
		if (collision != nullptr) {
			collidingWithSphere.push_back(((Twin2Engine::Physic::GameCollider*)collision->otherCollider)->colliderComponent);
			delete collision;
		}
	}

	for (auto other : IgnoreCollisionLayer) {
		collision = Twin2Engine::Physic::Collider::testCollision(collider, other, false);
		if (collision != nullptr) {
			collidingWithSphere.push_back(((Twin2Engine::Physic::GameCollider*)collision->otherCollider)->colliderComponent);
			delete collision;
		}
	}

	for (auto other : Layer1) {
		collision = Twin2Engine::Physic::Collider::testCollision(collider, other, false);
		if (collision != nullptr) {
			collidingWithSphere.push_back(((Twin2Engine::Physic::GameCollider*)collision->otherCollider)->colliderComponent);
			delete collision;
		}
	}

	for (auto other : Layer2) {
		collision = Twin2Engine::Physic::Collider::testCollision(collider, other, false);
		if (collision != nullptr) {
			collidingWithSphere.push_back(((Twin2Engine::Physic::GameCollider*)collision->otherCollider)->colliderComponent);
			delete collision;
		}
	}

	for (auto other : Layer3) {
		collision = Twin2Engine::Physic::Collider::testCollision(collider, other, false);
		if (collision != nullptr) {
			collidingWithSphere.push_back(((Twin2Engine::Physic::GameCollider*)collision->otherCollider)->colliderComponent);
			delete collision;
		}
	}

	for (auto other : Layer4) {
		collision = Twin2Engine::Physic::Collider::testCollision(collider, other, false);
		if (collision != nullptr) {
			collidingWithSphere.push_back(((Twin2Engine::Physic::GameCollider*)collision->otherCollider)->colliderComponent);
			delete collision;
		}
	}

	delete collider;
}
