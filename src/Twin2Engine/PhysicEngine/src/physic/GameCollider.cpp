#include <physic/GameCollider.h>
#include <core/Transform.h>

//#define USE_BOUNDING_VOLUMES

using namespace Twin2Engine::Physic;

//LastFrameCollisions

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

GameCollider::GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, HexagonalColliderData* hexagonalColliderData)
	: colliderComponent(colliderComponent) {
	colliderShape = ColliderShape::HEXAGONAL;
	shapeColliderData = hexagonalColliderData;
}

GameCollider::~GameCollider() {
	LastFrameCollisions.clear();
	colliderComponent = nullptr;
	boundingVolume = nullptr;
}

Collision* GameCollider::testBoundingVolume(BoundingVolume* other) const {
	if (other != nullptr) {
		if (boundingVolume != nullptr) {
			return testCollision(this->boundingVolume, other, false);
		}
		else {
			return testCollision((Collider*)this, other, false);
		}
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
	else if (boundingVolume != nullptr) {
		return testCollision(boundingVolume, other, false);
	}
	else {
		return new Collision;
	}
}

Collision* GameCollider::testColliders(GameCollider* collider) const {
	return testCollision((Collider*)this, collider, !(isTrigger || collider->isTrigger));
}

Collision* GameCollider::collide(Collider* other) {
	if (other->isBoundingVolume) {
		return testBoundingVolume((BoundingVolume*)other);
	}
	else if (enabled && ((GameCollider*)other)->enabled) {

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
					SPDLOG_INFO("{} - OnTriggerEnter", colliderId);
					Collision* col = new Collision();
					col->collider = (GameCollider*)collision->collider;
					col->otherCollider = (GameCollider*)collision->otherCollider;
					OnTriggerEnter.Invoke(col);
					delete col;

					col = new Collision();
					col->collider = (GameCollider*)collision->otherCollider;
					col->otherCollider = (GameCollider*)collision->collider;
					((GameCollider*)other)->OnTriggerEnter.Invoke(col);
					delete col;
				}
				else {
					if (isStatic) {
						if (((GameCollider*)other)->isStatic) {
							//separacja obu gameobjektów
							SPDLOG_INFO("{} - separacja obu gameobjektów ({}, {}, {})", colliderId, collision->separation.x,
								collision->separation.y, collision->separation.z);
							colliderComponent->GetTransform()->SetGlobalPosition(
								colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
							((GameCollider*)other)->colliderComponent->GetTransform()->SetGlobalPosition(
								((GameCollider*)other)->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
						}
						else {
							//separacje drugiego gameobjekta
							SPDLOG_INFO("{} - separacje drugiego gameobjekta ({}, {}, {})", colliderComponent->colliderId, collision->separation.x,
								collision->separation.y, collision->separation.z);
							//std::cout << colliderComponent->colliderId << " - separacje drugiego gameobjekta\n";
							((GameCollider*)other)->colliderComponent->GetTransform()->SetGlobalPosition(
								((GameCollider*)other)->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
						}
					}
					else {
						if (((GameCollider*)other)->isStatic) {
							//separacja tego game objekta
							SPDLOG_INFO("{} - separacja tego game objekta ({}, {}, {})", colliderComponent->colliderId, collision->separation.x,
								collision->separation.y, collision->separation.z);
							//std::cout << colliderComponent->colliderId << " - separacja tego game objekta\n";
							colliderComponent->GetTransform()->SetGlobalPosition(
								colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
						}
						else {
							//separacja obu gameobjektów
							SPDLOG_INFO("{} - separacja obu gameobjektów \t({} {})\t ({}, {}, {})", colliderComponent->colliderId, colliderShape, other->colliderShape, collision->separation.x,
								collision->separation.y, collision->separation.z);
							//std::cout << colliderComponent->colliderId << " - separacja obu gameobjektów\n";
							colliderComponent->GetTransform()->SetGlobalPosition(
								colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
							((GameCollider*)other)->colliderComponent->GetTransform()->SetGlobalPosition(
								((GameCollider*)other)->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
						}
					}/**/

					//Execute OnCollisionEnter
					SPDLOG_INFO("{} - OnCollisionEnter", colliderId);
					Collision* col = new Collision();
					col->collider = (GameCollider*)collision->collider;
					col->otherCollider = (GameCollider*)collision->otherCollider;
					col->separation = collision->separation;
					OnCollisionEnter.Invoke(col);
					delete col;

					col = new Collision();
					col->collider = (GameCollider*)collision->otherCollider;
					col->otherCollider = (GameCollider*)collision->collider;
					col->separation = -collision->separation;
					((GameCollider*)other)->OnCollisionEnter.Invoke(col);
					delete col;
				}
			}
		}
		else {
			if (LastFrameCollisions.contains((GameCollider*)other)) {
				LastFrameCollisions.erase((GameCollider*)other);

				if (isTrigger) {
					//Execute OnTriggerExit
					SPDLOG_INFO("{} - OnTriggerExit", colliderId);
					OnTriggerExit.Invoke((GameCollider*)other);
					((GameCollider*)other)->OnTriggerExit.Invoke(this);
				}
				else {
					//Execute OnCollisionExit
					SPDLOG_INFO("{} - OnCollisionExit", colliderId);
					OnCollisionExit.Invoke((GameCollider*)other);
					((GameCollider*)other)->OnCollisionExit.Invoke(this);
				}
			}
		}

		return collision;
	}

	return nullptr;
};

bool lineSegmentIntersection(const glm::vec3& p, const glm::vec3& d, const glm::vec3 a, const glm::vec3 b, glm::vec3& intersection) {
	glm::vec3 ab = b - a;
	glm::vec3 pa = a - p;

	// Solve the system of equations:
	// p + t * d = a + u * (b - a)
	// Rearranged to (d, -(b - a)) * (t, u) = a - p
	// Matrix form: [dx, -abx; dy, -aby; dz, -abz] * [t; u] = [ax - px; ay - py; az - pz]
	glm::vec3 h = glm::cross(d, ab);
	float det = glm::dot(h, pa);

	if (fabs(det) < 1e-8) // Check if the line and segment are parallel
		return false;

	float t = glm::dot(glm::cross(pa, ab), h) / det;
	float u = glm::dot(glm::cross(pa, d), h) / det;

	if (u < 0.0 || u > 1.0) // Check if the intersection is within the segment
		return false;

	intersection = p + d * t; // Compute the intersection point
	return true;
}

bool planeLineIntersection(const glm::vec3 normal, float d, const glm::vec3& p, const glm::vec3& dir, glm::vec3& intersection) {
	float denom = glm::dot(normal, dir);

	// Check if the line and plane are parallel
	if (fabs(denom) < 1e-8)
		return false;

	float t = -((glm::dot(normal, p) + d) / denom);

	// Check if the intersection point lies on the line
	if (t < 0.0)
		return false;

	intersection = p + dir * t; // Compute the intersection point
	return true;
}

bool GameCollider::rayCollision(Ray& ray, RaycastHit& raycastHit) {
	//Collision* collision = new Collision;
	raycastHit.collider = this->colliderComponent;

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
			raycastHit.position = relativePos;
		}
		else if (distSqr < rSqrt) {
			raycastHit.position = relativePos - glm::sqrt(rSqrt - distSqr) * ray.Direction;
		}
		else {
			return false;
		}
	}

	break;

	case ColliderShape::BOX:
	{
		BoxColliderData* boxData = (BoxColliderData*)shapeColliderData;
		glm::vec3 relativePos = ray.Origin - boxData->Position;
		relativePos = { glm::dot(relativePos, boxData->XAxis),
						glm::dot(relativePos, boxData->YAxis), 
						glm::dot(relativePos, boxData->ZAxis) };
		glm::vec3 relativeDir = glm::normalize(glm::vec3(glm::dot(ray.Direction, boxData->XAxis), 
														 glm::dot(ray.Direction, boxData->YAxis), 
														 glm::dot(ray.Direction, boxData->ZAxis)));

		float t;

		float tmin = (-boxData->HalfDimensions.x - relativePos.x) / relativeDir.x;
		float tmax = ( boxData->HalfDimensions.x - relativePos.x) / relativeDir.x;

		if (tmin > tmax) {
			t = tmin;
			tmin = tmax;
			tmax = t;
		};

		float tymin = (-boxData->HalfDimensions.y - relativePos.y) / relativeDir.y;
		float tymax = ( boxData->HalfDimensions.y - relativePos.y) / relativeDir.y;

		if (tymin > tymax) {
			t = tymin;
			tymin = tymax;
			tymax = t;
		};

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin) tmin = tymin;
		if (tymax < tmax) tmax = tymax;

		float tzmin = (-boxData->HalfDimensions.z - relativePos.z) / relativeDir.z;
		float tzmax = ( boxData->HalfDimensions.z - relativePos.z) / relativeDir.z;

		if (tzmin > tzmax) {
			t = tzmin;
			tzmin = tzmax;
			tzmax = t;
		};


		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin) tmin = tzmin;
		if (tzmax < tmax) tmax = tzmax;

		raycastHit.position = ray.Origin + ray.Direction * tmin;

		return true;
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
			return false;
		}

		float sqrtDiscriminant = sqrt(discriminant);
		float t1 = (-b - sqrtDiscriminant) / (2 * a);
		float t2 = (-b + sqrtDiscriminant) / (2 * a);

		// Find the smallest positive t
		float t = (t1 < t2 && t1 >= 0) ? t1 : t2;
		raycastHit.position = ray.Origin + ray.Direction * t;
	}
	break;

	case ColliderShape::HEXAGONAL:
	{
		HexagonalColliderData* hexData = (HexagonalColliderData*)shapeColliderData;
		glm::vec3 dir = -ray.Direction;
		dir.y = 0.0f;
		dir = glm::normalize(dir);
		float up = glm::dot(dir, hexData->u);
		float vp = glm::dot(dir, hexData->v);
		float wp = glm::dot(dir, hexData->w);

		glm::vec3 intersectionPoint(0.0f);
		glm::vec3 o = ray.Origin;
		o.y = hexData->Position.y;
		dir *= -1.0f;

		bool intersect = false;

		if (up >= 0.5 && vp >= 0.5) {
			if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->w), std::move(hexData->Position + hexData->u), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->u), std::move(hexData->Position + hexData->v), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->v), std::move(hexData->Position + hexData->w), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (vp >= 0.5 && wp >= 0.5) {
			if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->u), std::move(hexData->Position + hexData->v), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->v), std::move(hexData->Position + hexData->w), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->w), std::move(hexData->Position - hexData->u), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (wp >= 0.5 && up <= -0.5) {
			if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->v), std::move(hexData->Position + hexData->w), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->w), std::move(hexData->Position - hexData->u), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->u), std::move(hexData->Position - hexData->v), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (up <= -0.5 && vp <= -0.5) {
			if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->w), std::move(hexData->Position - hexData->u), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->u), std::move(hexData->Position - hexData->v), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->v), std::move(hexData->Position - hexData->w), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (vp <= -0.5 && wp <= -0.5) {
			if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->u), std::move(hexData->Position - hexData->v), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->v), std::move(hexData->Position - hexData->w), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->w), std::move(hexData->Position + hexData->u), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (wp <= -0.5 && up >= 0.5) {
			if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->v), std::move(hexData->Position - hexData->w), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position - hexData->w), std::move(hexData->Position + hexData->u), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection(o, dir, std::move(hexData->Position + hexData->u), std::move(hexData->Position + hexData->v), intersectionPoint)) {
				intersect = true;
			}
		}

		glm::vec3 baseIntersectionPoint(0.0f);
		bool baseIntersec = false;
		if (ray.Origin.y > (hexData->Position.y + hexData->HalfHeight)) {
			o = hexData->Position;
			o.y += hexData->HalfHeight;
			baseIntersec = planeLineIntersection(glm::vec3(0.0f, 1.0f, 0.0f), o.y, ray.Origin, ray.Direction, baseIntersectionPoint);
		}
		else if (ray.Origin.y > (hexData->Position.y - hexData->HalfHeight)) {
			o = hexData->Position;
			o.y -= hexData->HalfHeight;
			baseIntersec = planeLineIntersection(glm::vec3(0.0f, -1.0f, 0.0f), o.y, ray.Origin, ray.Direction, baseIntersectionPoint);
		}

		if ((!baseIntersec) && intersect) {
			raycastHit.position = intersectionPoint;
			return true;
		}

		if (baseIntersec) {
			float dist1Sqr = (hexData->Position.x - intersectionPoint.x) * (hexData->Position.x - intersectionPoint.x) + (hexData->Position.y - intersectionPoint.y) * (hexData->Position.y - intersectionPoint.y);
			float dist2Sqr = (hexData->Position.x - baseIntersectionPoint.x) * (hexData->Position.x - baseIntersectionPoint.x) + (hexData->Position.y - baseIntersectionPoint.y) * (hexData->Position.y - baseIntersectionPoint.y);

			if (dist2Sqr < dist1Sqr) {
				intersectionPoint = baseIntersectionPoint;
			}
			else {
				if (!lineSegmentIntersection(ray.Origin, ray.Direction,
					std::move(glm::vec3(hexData->Position.x, hexData->Position.y + hexData->HalfHeight, hexData->Position.z)),
					std::move(glm::vec3(hexData->Position.x, hexData->Position.y - hexData->HalfHeight, hexData->Position.z)), intersectionPoint)) {
					return false;
				}
			}

			raycastHit.position = intersectionPoint;
			return true;
		}
		return false;
	}
	break;
	}

	return true;
}