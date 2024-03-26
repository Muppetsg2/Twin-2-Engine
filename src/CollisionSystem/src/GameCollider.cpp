#include "GameCollider.h"
#include "core/GameObject.h"

#include <iostream>
#include <spdlog/spdlog.h>

//#define USE_BOUNDING_VOLUMES

using namespace CollisionSystem;

GameCollider::GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, SphereColliderData* sphereColliderData)
						: colliderComponent(colliderComponent) {
	colliderShape = ColliderShape::SPHERE;
	shapeColliderData = sphereColliderData;
}

GameCollider::GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, BoxColliderData* boxColliderData)
						: colliderComponent(colliderComponent) {
	colliderShape = ColliderShape::BOX;
	shapeColliderData = boxColliderData;
}

GameCollider::GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, CapsuleColliderData* capsuleColliderData)
						: colliderComponent(colliderComponent) {
	colliderShape = ColliderShape::CAPSULE;
	shapeColliderData = capsuleColliderData;
}

GameCollider::~GameCollider() {
	delete boundingVolume;
}


Collision* GameCollider::testBoundingVolume(BoundingVolume* other) const {
	if (this->boundingVolume != nullptr && boundingVolume != nullptr) {
		return testCollision(this->boundingVolume, boundingVolume, false);
	}
	else if (boundingVolume == nullptr && boundingVolume != nullptr) {
		return testCollision((Collider*)this, boundingVolume, false);
	}
	else {
		return nullptr;
	}
}

Collision* GameCollider::testBoundingVolume(GameCollider* other) const {
	if (boundingVolume != nullptr && other->boundingVolume != nullptr) {
		return testCollision(boundingVolume, other->boundingVolume, false);
	}
	else if (boundingVolume == nullptr && other->boundingVolume != nullptr) {
		return testCollision((Collider*)this, other->boundingVolume, false);
	}
	else {
		return testCollision(boundingVolume, other, false);
	}
}

Collision* GameCollider::testColliders(GameCollider* collider) const {
	return testCollision((Collider*)this, collider, !(isTrigger || isTrigger));
}

Collision* GameCollider::collide(Collider* other) {
	if (other->isBoundingVolume) {
		return testBoundingVolume((BoundingVolume*)other);
	}
	else {

#ifdef USE_BOUNDING_VOLUMES
		Collision* collision = nullptr;// testBoundingVolume(((GameCollider*)other)->boundingVolume);

		if (boundingVolume == nullptr && ((GameCollider*)other)->boundingVolume == nullptr) {
			collision = testColliders((GameCollider*)other);
		}
		else {
			if (testBoundingVolume((GameCollider*)other) != nullptr) {
				collision = testColliders((GameCollider*)other);
			}
		}

#else
		Collision* collision = testColliders((GameCollider*)other);
#endif // USE_BOUNDING_VOLUMES
		
		if (collision != nullptr) {
			if (!LastFrameCollisions.contains((GameCollider*)other)) {
				LastFrameCollisions.insert((GameCollider*)other);

				if (isTrigger) {
					//Execute OnTriggerEnter
					SPDLOG_INFO("{} - OnTriggerEnter\n", colliderComponent->colliderId);
					//std::cout << colliderComponent->colliderId << " - OnTriggerEnter\n";
					Twin2Engine::Core::Collision* col = new Twin2Engine::Core::Collision;
					col->collider = ((GameCollider*)collision->collider)->colliderComponent;
					col->otherCollider = ((GameCollider*)collision->otherCollider)->colliderComponent;
					col->position = collision->position;
					colliderComponent->OnTriggerEnter.Invoke(col);
					delete col;

					col = new Twin2Engine::Core::Collision;
					col->collider = ((GameCollider*)collision->otherCollider)->colliderComponent;
					col->otherCollider = ((GameCollider*)collision->collider)->colliderComponent;
					col->position = collision->position;
					((GameCollider*)other)->colliderComponent->OnTriggerEnter.Invoke(col);
					delete col;
				}
				else {
					if (isStatic) {
						if (((GameCollider*)other)->isStatic) {
							//separacja obu gameobjekt�w
							SPDLOG_INFO("{} - separacja obu game objekt�w\n", colliderComponent->colliderId);
							//std::cout << colliderComponent->colliderId << " - separacja obu gameobjekt�w\n";
							colliderComponent->getGameObject()->GetTransform()->SetGlobalPosition(
								colliderComponent->getGameObject()->GetTransform()->GetGlobalPosition() + collision->separation);
							((GameCollider*)other)->colliderComponent->getGameObject()->GetTransform()->SetGlobalPosition(
								colliderComponent->getGameObject()->GetTransform()->GetGlobalPosition() - collision->separation);
						}
						else {
							//separacje drugiego gameobjekta
							SPDLOG_INFO("{} - separacja tego game objekta\n", colliderComponent->colliderId);
							//std::cout << colliderComponent->colliderId << " - separacje drugiego gameobjekta\n";
							((GameCollider*)other)->colliderComponent->getGameObject()->GetTransform()->SetGlobalPosition(
								colliderComponent->getGameObject()->GetTransform()->GetGlobalPosition() - collision->separation);
						}
					}
					else {
						if (((GameCollider*)other)->isStatic) {
							//separacja tego game objekta
							SPDLOG_INFO("{} - separacja tego game objekta\n", colliderComponent->colliderId);
							//std::cout << colliderComponent->colliderId << " - separacja tego game objekta\n";
							colliderComponent->getGameObject()->GetTransform()->SetGlobalPosition(
								colliderComponent->getGameObject()->GetTransform()->GetGlobalPosition() + collision->separation);
						}
						else {
							//separacja obu gameobjekt�w
							SPDLOG_INFO("{} - separacja obu gameobjekt�w\n", colliderComponent->colliderId);
							//std::cout << colliderComponent->colliderId << " - separacja obu gameobjekt�w\n";
							colliderComponent->getGameObject()->GetTransform()->SetGlobalPosition(
								colliderComponent->getGameObject()->GetTransform()->GetGlobalPosition() + collision->separation);
							((GameCollider*)other)->colliderComponent->getGameObject()->GetTransform()->SetGlobalPosition(
								colliderComponent->getGameObject()->GetTransform()->GetGlobalPosition() - collision->separation);
						}
					}

					//Execute OnCollisionEnter
					SPDLOG_INFO("{} - OnCollisionEnter\n", colliderComponent->colliderId);
					//std::cout << colliderComponent->colliderId << " - OnTriggerExit\n";
					Twin2Engine::Core::Collision* col = new Twin2Engine::Core::Collision;
					col->collider = ((GameCollider*)collision->collider)->colliderComponent;
					col->otherCollider = ((GameCollider*)collision->otherCollider)->colliderComponent;
					col->position = collision->position;
					colliderComponent->OnCollisionEnter.Invoke(col);
					delete col;

					col = new Twin2Engine::Core::Collision;
					col->collider = ((GameCollider*)collision->otherCollider)->colliderComponent;
					col->otherCollider = ((GameCollider*)collision->collider)->colliderComponent;
					col->position = collision->position;
					((GameCollider*)other)->colliderComponent->OnCollisionEnter.Invoke(col);
					delete col;
				}
			}
		}
		else {
			if (LastFrameCollisions.contains((GameCollider*)other)) {
				LastFrameCollisions.erase((GameCollider*)other);

				if (isTrigger) {
					//Execute OnTriggerExit
					SPDLOG_INFO("{} - OnTriggerExit\n", colliderComponent->colliderId);
					//std::cout << colliderComponent->colliderId << " - OnTriggerExit\n";
					colliderComponent->OnTriggerExit.Invoke(((GameCollider*)other)->colliderComponent);
					((GameCollider*)other)->colliderComponent->OnTriggerExit.Invoke(colliderComponent);
				}
				else {
					//Execute OnCollisionExit
					SPDLOG_INFO("{} - OnCollisionExit\n", colliderComponent->colliderId);
					//std::cout << colliderComponent->colliderId << " - OnCollisionExit\n";
					colliderComponent->OnCollisionExit.Invoke(((GameCollider*)other)->colliderComponent);
					((GameCollider*)other)->colliderComponent->OnCollisionExit.Invoke(colliderComponent);
				}
			}
		}

		return collision;
	}

	return nullptr;
};

Collision* GameCollider::rayCollision(Ray& ray) {
	Collision* collision = new Collision;
	collision->collider = this;

	switch (colliderShape) {
		case ColliderShape::SPHERE:
			{
				SphereColliderData* sphereData = (SphereColliderData*)shapeColliderData;
				glm::vec3 relativePos = sphereData->Position - ray.Origin;
				relativePos = glm::dot(relativePos, ray.Direction) * ray.Direction + ray.Origin;
				glm::vec3 diff = sphereData->Position - relativePos;
				float distSqr = glm::dot(diff, diff);
				float rSqrt = sphereData->Radius * sphereData->Radius;

				if (distSqr == rSqrt) {
					collision->position = relativePos;
				}
				else if (distSqr < rSqrt) {
					collision->position = relativePos - glm::sqrt(rSqrt - distSqr) * ray.Direction;
				}
			}

			break;

		case ColliderShape::BOX:
			{
				BoxColliderData* boxData = (BoxColliderData*)shapeColliderData;
				//glm::vec3 relativePos = boxData->Position - ray.Origin;
				float tmin = - 1.0f;
				float t;

				float dn = glm::dot(ray.Direction, boxData->XAxis);
				if (dn != 0.0f) {
					tmin = (boxData->HalfDimensions.x - glm::dot(ray.Origin, boxData->XAxis)) / dn;
				}
				dn = glm::dot(ray.Direction, -boxData->XAxis);
				if (dn != 0.0f) {
					t = (boxData->HalfDimensions.x - glm::dot(ray.Origin, -boxData->XAxis)) / dn;
					if (t < tmin) {
						tmin = t;
					}
				}

				dn = glm::dot(ray.Direction, boxData->YAxis);
				if (dn != 0.0f) {
					t = (boxData->HalfDimensions.y - glm::dot(ray.Origin, boxData->YAxis)) / dn;
					if (t < tmin) {
						tmin = t;
					}
				}
				dn = glm::dot(ray.Direction, -boxData->YAxis);
				if (dn != 0.0f) {
					t = (boxData->HalfDimensions.y - glm::dot(ray.Origin, -boxData->YAxis)) / dn;
					if (t < tmin) {
						tmin = t;
					}
				}

				dn = glm::dot(ray.Direction, boxData->ZAxis);
				if (dn != 0.0f) {
					t = (boxData->HalfDimensions.z - glm::dot(ray.Origin, boxData->ZAxis)) / dn;
					if (t < tmin) {
						tmin = t;
					}
				}
				dn = glm::dot(ray.Direction, -boxData->ZAxis);
				if (dn != 0.0f) {
					t = (boxData->HalfDimensions.z - glm::dot(ray.Origin, -boxData->ZAxis)) / dn;
					if (t < tmin) {
						tmin = t;
					}
				}

				collision->position = ray.Origin + ray.Direction * tmin;
			}

			break;

		case ColliderShape::CAPSULE:
			{
				CapsuleColliderData* capsuleData = (CapsuleColliderData*)shapeColliderData;
				glm::vec3 ab = capsuleData->EndPosition - capsuleData->Position;
				glm::vec3 ao = ray.Origin - capsuleData->Position;
				glm::vec3 ao_x_ab = glm::cross(ao, ab);
				glm::vec3 v_x_ab = glm::cross(ray.Direction, ab);
				float ab_dot_d = glm::dot(ab, ray.Direction);
				float ab_dot_ao = glm::dot(ab, ao);
				

				float m = ab_dot_d / glm::dot(ab, ab);
				float n = ab_dot_ao / glm::dot(ab, ab);

				glm::vec3 Q = ray.Direction - (ab * m);
				glm::vec3 R = ao - (ab * n);

				float a = glm::dot(Q, Q);
				float b = 2.0f * glm::dot(Q, R);
				float c = glm::dot(R, R) - (capsuleData->Radius * capsuleData->Radius);

				// Solve quadratic equation for t
				float discriminant = b * b - 4 * a * c;
				if (discriminant < 0) {
					collision = nullptr;
					break; // No real roots, no intersection
				}

				float sqrtDiscriminant = sqrt(discriminant);
				float t1 = (-b - sqrtDiscriminant) / (2 * a);
				float t2 = (-b + sqrtDiscriminant) / (2 * a);

				// Find the smallest positive t
				float t = (t1 < t2 && t1 >= 0) ? t1 : t2;
				collision->position = ray.Origin + ray.Direction * t;
			}
			break;
	}

	return collision;
}