#include <physic/GameCollider.h>
#include <core/HexagonalColliderComponent.h>
#include <core/Transform.h>

//#define USE_BOUNDING_VOLUMES

using namespace Twin2Engine::Physic;

//LastFrameCollisions

GameCollider::GameCollider(Twin2Engine::Core::ColliderComponent* colliderComponent, ColliderShape shapeData)
	: colliderComponent(colliderComponent) {
	colliderShape = shapeData;

	switch (shapeData) {
	case ColliderShape::BOX:
		shapeColliderData = new BoxColliderData();
		break;
	case ColliderShape::SPHERE:
		shapeColliderData = new SphereColliderData();
		break;
	case ColliderShape::CAPSULE:
		shapeColliderData = new CapsuleColliderData();
		break;
	case ColliderShape::HEXAGONAL:
		shapeColliderData = new HexagonalColliderData();
		break;
	default:
		shapeColliderData = new SphereColliderData();
		break;
	}
}

GameCollider::~GameCollider() {
	LastFrameCollisions.clear();
	colliderComponent = nullptr;
}

Collision* GameCollider::testColliders(GameCollider* collider) const {
	return testCollision((Collider*)this, (Collider*)collider, !(isTrigger || collider->isTrigger));
}

Collision* GameCollider::collide(Collider* other) {

	if (enabled && ((GameCollider*)other)->enabled) {

		Collision* collision = testColliders((GameCollider*)other);

		if (collision != nullptr) {
			if (!LastFrameCollisions.contains((GameCollider*)other)) {
				LastFrameCollisions.insert((GameCollider*)other);


				if (isTrigger) {
					//Execute OnTriggerEnter
					//SPDLOG_INFO("{} - OnTriggerEnter", colliderComponent->colliderId);
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
							//separacja obu gameobjekt�w
							//SPDLOG_INFO("{} - separacja obu gameobjekt�w ({}, {}, {})", colliderComponent->colliderId, collision->separation.x, collision->separation.y, collision->separation.z);
							colliderComponent->GetTransform()->SetGlobalPosition(
								colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
							((GameCollider*)other)->colliderComponent->GetTransform()->SetGlobalPosition(
								((GameCollider*)other)->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
						}
						else {
							//separacje drugiego gameobjekta
							//SPDLOG_INFO("{} - separacje drugiego gameobjekta ({}, {}, {})", colliderComponent->colliderId, collision->separation.x, collision->separation.y, collision->separation.z);
							((GameCollider*)other)->colliderComponent->GetTransform()->SetGlobalPosition(
								((GameCollider*)other)->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
						}
					}
					else {
						if (((GameCollider*)other)->isStatic) {
							//separacja tego game objekta
							//SPDLOG_INFO("{} - separacja tego game objekta ({}, {}, {})", colliderComponent->colliderId, collision->separation.x, collision->separation.y, collision->separation.z);
							colliderComponent->GetTransform()->SetGlobalPosition(
								colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
						}
						else {
							//separacja obu gameobjekt�w
							//SPDLOG_INFO("{} - separacja obu gameobjekt�w \t({} {})\t ({}, {}, {})", colliderComponent->colliderId, Twin2Engine::Physic::to_string(colliderShape), Twin2Engine::Physic::to_string(other->colliderShape), collision->separation.x, collision->separation.y, collision->separation.z);
							colliderComponent->GetTransform()->SetGlobalPosition(
								colliderComponent->GetTransform()->GetGlobalPosition() + collision->separation);
							((GameCollider*)other)->colliderComponent->GetTransform()->SetGlobalPosition(
								((GameCollider*)other)->colliderComponent->GetTransform()->GetGlobalPosition() - collision->separation);
						}
					}/**/

					//Execute OnCollisionEnter
					//SPDLOG_INFO("{} - OnCollisionEnter", colliderComponent->colliderId);
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
					//SPDLOG_INFO("{} - OnTriggerExit", colliderComponent->colliderId);
					OnTriggerExit.Invoke((GameCollider*)other);
					((GameCollider*)other)->OnTriggerExit.Invoke(this);
				}
				else {
					//Execute OnCollisionExit
					//SPDLOG_INFO("{} - OnCollisionExit", colliderComponent->colliderId);
					OnCollisionExit.Invoke((GameCollider*)other);
					((GameCollider*)other)->OnCollisionExit.Invoke(this);
				}
			}
		}

		return collision;
	}

	return nullptr;
};

bool lineSegmentIntersection3D(const glm::vec3& p, const glm::vec3& d, const glm::vec3 a, const glm::vec3 b, glm::vec3& intersection) {
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

bool lineSegmentIntersection2D(const glm::vec3& p, const glm::vec3& d, const glm::vec3 a, const glm::vec3 b, glm::vec3& intersection) {
	glm::vec3 AB = b - a;
	double determinant = d.x * (-AB.z) - d.z * (-AB.x);

	if (determinant == 0) {
		// Lines are parallel
		return false;
	}

	glm::vec3 PA = a - p;
	float t = (PA.x * (-AB.z) - PA.z * (-AB.x)) / determinant;
	float u = (d.x * PA.z - d.z * PA.x) / determinant;

	if (u >= 0.0 && u <= 1.0) {
		intersection = p + d * t;
		return true;
	}

	return false;
}

bool planeLineIntersection(const glm::vec3 normal, float d, const glm::vec3& origin, const glm::vec3& dir, glm::vec3& intersection) {
	float denom = glm::dot(normal, dir);

	// Check if the line and plane are parallel
	if (fabs(denom) < 1e-8)
		return false;

	//float t = -((glm::dot(normal, p) + d) / denom);

	float t = glm::dot(normal * d - origin, normal) / glm::dot(dir, normal);

	// Check if the intersection point lies on the line
	if (t < 0.0)
		return false;

	intersection = origin + dir * t; // Compute the intersection point
	return true;
}

bool pointInTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
	glm::vec2 v0 = c - a;
	glm::vec2 v1 = b - a;
	glm::vec2 v2 = p - a;

	float dot00 = v0.x * v0.x + v0.y * v0.y;
	float dot01 = v0.x * v1.x + v0.y * v1.y;
	float dot02 = v0.x * v2.x + v0.y * v2.y;
	float dot11 = v1.x * v1.x + v1.y * v1.y;
	float dot12 = v1.x * v2.x + v1.y * v2.y;

	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u >= 0) && (v >= 0) && (u + v < 1);
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
		/*/HexagonalColliderData* hexData = (HexagonalColliderData*)shapeColliderData;
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
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->w * hexData->BaseLength), std::move(hexData->Position + hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->u * hexData->BaseLength), std::move(hexData->Position + hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->v * hexData->BaseLength), std::move(hexData->Position + hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (vp >= 0.5 && wp >= 0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->u * hexData->BaseLength), std::move(hexData->Position + hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->v * hexData->BaseLength), std::move(hexData->Position + hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->w * hexData->BaseLength), std::move(hexData->Position - hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (wp >= 0.5 && up <= -0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->v * hexData->BaseLength), std::move(hexData->Position + hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->w * hexData->BaseLength), std::move(hexData->Position - hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->u * hexData->BaseLength), std::move(hexData->Position - hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (up <= -0.5 && vp <= -0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->w * hexData->BaseLength), std::move(hexData->Position - hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->u * hexData->BaseLength), std::move(hexData->Position - hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->v * hexData->BaseLength), std::move(hexData->Position - hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (vp <= -0.5 && wp <= -0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->u * hexData->BaseLength), std::move(hexData->Position - hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->v * hexData->BaseLength), std::move(hexData->Position - hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->w * hexData->BaseLength), std::move(hexData->Position + hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (wp <= -0.5 && up >= 0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->v * hexData->BaseLength), std::move(hexData->Position - hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->w * hexData->BaseLength), std::move(hexData->Position + hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->u * hexData->BaseLength), std::move(hexData->Position + hexData->v * hexData->BaseLength), intersectionPoint)) {
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
		else if (ray.Origin.y < (hexData->Position.y - hexData->HalfHeight)) {
			o = hexData->Position;
			o.y -= hexData->HalfHeight;
			baseIntersec = planeLineIntersection(glm::vec3(0.0f, -1.0f, 0.0f), o.y, ray.Origin, ray.Direction, baseIntersectionPoint);
		}
		else if (ray.Direction.y > 0.0f) {
			o = hexData->Position;
			o.y += hexData->HalfHeight;
			baseIntersec = planeLineIntersection(glm::vec3(0.0f, 1.0f, 0.0f), o.y, ray.Origin, ray.Direction, baseIntersectionPoint);
		}
		else if (ray.Direction.y < 0.0f) {
			o = hexData->Position;
			o.y -= hexData->HalfHeight;
			baseIntersec = planeLineIntersection(glm::vec3(0.0f, -1.0f, 0.0f), o.y, ray.Origin, ray.Direction, baseIntersectionPoint);
		}

		if ((!baseIntersec) && intersect) {
			raycastHit.position = intersectionPoint;
			return true;
		}

		if (baseIntersec && intersect) {
			float dist1Sqr = (hexData->Position.x - intersectionPoint.x) * (hexData->Position.x - intersectionPoint.x) + (hexData->Position.z - intersectionPoint.z) * (hexData->Position.z - intersectionPoint.z);
			float dist2Sqr = (hexData->Position.x - baseIntersectionPoint.x) * (hexData->Position.x - baseIntersectionPoint.x) + (hexData->Position.z - baseIntersectionPoint.z) * (hexData->Position.z - baseIntersectionPoint.z);

			if (dist2Sqr < dist1Sqr) {
				intersectionPoint = baseIntersectionPoint;
			}
			else {
				if (!lineSegmentIntersection3D(ray.Origin, ray.Direction,
					std::move(glm::vec3(intersectionPoint.x, hexData->Position.y + hexData->HalfHeight, intersectionPoint.z)),
					std::move(glm::vec3(intersectionPoint.x, hexData->Position.y - hexData->HalfHeight, intersectionPoint.z)), intersectionPoint)) {
					return false;
				}
			}

			raycastHit.position = intersectionPoint;
			return true;
		}

		if (baseIntersec) {
			glm::vec2 p(baseIntersectionPoint.x, baseIntersectionPoint.z);
			glm::vec2 p1(hexData->Position.x + hexData->u.x, hexData->Position.z + hexData->u.z);
			glm::vec2 p2(hexData->Position.x + hexData->v.x, hexData->Position.z + hexData->v.z);
			glm::vec2 p3(hexData->Position.x + hexData->w.x, hexData->Position.z + hexData->w.z);
			glm::vec2 p4(hexData->Position.x - hexData->u.x, hexData->Position.z - hexData->u.z);
			glm::vec2 p5(hexData->Position.x - hexData->v.x, hexData->Position.z - hexData->v.z);
			glm::vec2 p6(hexData->Position.x - hexData->w.x, hexData->Position.z - hexData->w.z);

			if (pointInTriangle(p, p1, p2, p3) || pointInTriangle(p, p4, p5, p6) || pointInTriangle(p, p1, p3, p4) || pointInTriangle(p, p4, p6, p1)) {
				raycastHit.position = baseIntersectionPoint;
				return true;
			}
		}

		return false;/**/
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
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->w * hexData->BaseLength), std::move(hexData->Position + hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->u * hexData->BaseLength), std::move(hexData->Position + hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->v * hexData->BaseLength), std::move(hexData->Position + hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (vp >= 0.5 && wp >= 0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->u * hexData->BaseLength), std::move(hexData->Position + hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->v * hexData->BaseLength), std::move(hexData->Position + hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->w * hexData->BaseLength), std::move(hexData->Position - hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (wp >= 0.5 && up <= -0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->v * hexData->BaseLength), std::move(hexData->Position + hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->w * hexData->BaseLength), std::move(hexData->Position - hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->u * hexData->BaseLength), std::move(hexData->Position - hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (up <= -0.5 && vp <= -0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->w * hexData->BaseLength), std::move(hexData->Position - hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->u * hexData->BaseLength), std::move(hexData->Position - hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->v * hexData->BaseLength), std::move(hexData->Position - hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (vp <= -0.5 && wp <= -0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->u * hexData->BaseLength), std::move(hexData->Position - hexData->v * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->v * hexData->BaseLength), std::move(hexData->Position - hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->w * hexData->BaseLength), std::move(hexData->Position + hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
		}
		else if (wp <= -0.5 && up >= 0.5) {
			if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->v * hexData->BaseLength), std::move(hexData->Position - hexData->w * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position - hexData->w * hexData->BaseLength), std::move(hexData->Position + hexData->u * hexData->BaseLength), intersectionPoint)) {
				intersect = true;
			}
			else if (lineSegmentIntersection2D(o, dir, std::move(hexData->Position + hexData->u * hexData->BaseLength), std::move(hexData->Position + hexData->v * hexData->BaseLength), intersectionPoint)) {
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
		else if (ray.Origin.y < (hexData->Position.y - hexData->HalfHeight)) {
			o = hexData->Position;
			o.y -= hexData->HalfHeight;
			baseIntersec = planeLineIntersection(glm::vec3(0.0f, -1.0f, 0.0f), o.y, ray.Origin, ray.Direction, baseIntersectionPoint);
		}
		
		if ((!baseIntersec) && intersect) {
			raycastHit.position = intersectionPoint;
			return true;
		}
		
		if (baseIntersec && intersect) {
			float dist1Sqr = (hexData->Position.x - intersectionPoint.x) * (hexData->Position.x - intersectionPoint.x) + (hexData->Position.z - intersectionPoint.z) * (hexData->Position.z - intersectionPoint.z);
			float dist2Sqr = (hexData->Position.x - baseIntersectionPoint.x) * (hexData->Position.x - baseIntersectionPoint.x) + (hexData->Position.z - baseIntersectionPoint.z) * (hexData->Position.z - baseIntersectionPoint.z);
		
			if (dist2Sqr < dist1Sqr) {
				intersectionPoint = baseIntersectionPoint;
			}
			else {
				if (!lineSegmentIntersection3D(ray.Origin, ray.Direction,
					std::move(glm::vec3(intersectionPoint.x, hexData->Position.y + hexData->HalfHeight, intersectionPoint.z)),
					std::move(glm::vec3(intersectionPoint.x, hexData->Position.y - hexData->HalfHeight, intersectionPoint.z)), intersectionPoint)) {
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