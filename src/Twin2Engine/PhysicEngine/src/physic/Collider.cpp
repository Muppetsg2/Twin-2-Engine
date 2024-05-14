#include <physic/Collider.h>

using namespace Twin2Engine::Physic;

Collider::~Collider() {
	delete shapeColliderData;
}

Collision* Collider::SphereSphereCollision(Collider* sphere1, Collider* sphere2, bool separate)
{
	SphereColliderData* sphereData1 = (SphereColliderData*)sphere1->shapeColliderData;
	SphereColliderData* sphereData2 = (SphereColliderData*)sphere2->shapeColliderData;

	float distanceSqr = (sphereData1->Position.x - sphereData2->Position.x) * (sphereData1->Position.x - sphereData2->Position.x) +
		(sphereData1->Position.y - sphereData2->Position.y) * (sphereData1->Position.y - sphereData2->Position.y) +
		(sphereData1->Position.z - sphereData2->Position.z) * (sphereData1->Position.z - sphereData2->Position.z);

	float radiusSum = sphereData1->Radius + sphereData2->Radius;
	if (distanceSqr <= (radiusSum * radiusSum)) {
		Collision* collision = new Collision;
		collision->collider = sphere1;
		collision->otherCollider = sphere2;
		//collision->position = (sphereData1->Radius * sphereData1->Position + sphereData2->Radius * sphereData2->Position) / radiusSum;

		if (separate) {
			collision->separation = glm::normalize(sphereData1->Position - sphereData2->Position) * ((radiusSum - glm::sqrt(distanceSqr)) / 2.0f);
		}

		return collision;
	}
	else {
		return nullptr;
	}
}

Collision* Collider::BoxBoxCollision(Collider* box1, Collider* box2, bool separate)
{
	BoxColliderData* boxData1 = (BoxColliderData*)box1->shapeColliderData;
	BoxColliderData* boxData2 = (BoxColliderData*)box2->shapeColliderData;

	glm::vec3 relativePosition = boxData2->Position - boxData1->Position;
	float distance = glm::sqrt(glm::dot(relativePosition, relativePosition));

	Collision* collision = new Collision;

	if (separate) {
		//collision->separation = glm::vec3(0.0f, 0.0f, 0.0f);
		relativePosition = glm::vec3(glm::dot(relativePosition, boxData1->XAxis),
			glm::dot(relativePosition, boxData1->YAxis),
			glm::dot(relativePosition, boxData1->ZAxis));
		float x = glm::abs(relativePosition.x);
		float y = glm::abs(relativePosition.y);
		float z = glm::abs(relativePosition.z);

		collision->separation = glm::vec3(0.0f, 0.0f, 0.0f);

		if (x > y && x > z) {
			if (relativePosition.x >= 0) {
				collision->separation.x = -1.0f;
			}
			else {
				collision->separation.x = 1.0f;
			}
		}
		else if (y > x && y > z) {
			if (relativePosition.y >= 0) {
				collision->separation.y = -1.0f;
			}
			else {
				collision->separation.y = 1.0f;
			}
		}
		else {
			if (relativePosition.z >= 0) {
				collision->separation.z = -1.0f;
			}
			else {
				collision->separation.z = 1.0f;
			}
		}
		//collision->separation = glm::normalize(sphereData1->Position - sphereData2->Position) * ((radiusSum - glm::sqrt(distanceSqr)) / 2.0f);
	}

	float projection = boxData2->HalfDimensions.x * glm::abs(glm::dot(boxData2->XAxis, boxData1->XAxis))
		+ boxData2->HalfDimensions.y * glm::abs(glm::dot(boxData2->YAxis, boxData1->XAxis))
		+ boxData2->HalfDimensions.z * glm::abs(glm::dot(boxData2->ZAxis, boxData1->XAxis));

	if ((projection + boxData1->HalfDimensions.x) <= glm::abs(glm::dot(relativePosition, boxData1->XAxis))) {
		return nullptr;
	}

	if (separate) {
		if (collision->separation.x != 0.0f) {
			collision->separation.x *= (projection + boxData1->HalfDimensions.x - distance) * 0.5f;
		}
	}

	projection = boxData2->HalfDimensions.x * glm::abs(glm::dot(boxData2->XAxis, boxData1->YAxis))
		+ boxData2->HalfDimensions.y * glm::abs(glm::dot(boxData2->YAxis, boxData1->YAxis))
		+ boxData2->HalfDimensions.z * glm::abs(glm::dot(boxData2->ZAxis, boxData1->YAxis));

	if ((projection + boxData1->HalfDimensions.y) <= glm::abs(glm::dot(relativePosition, boxData1->YAxis))) {
		return nullptr;
	}

	if (separate) {
		if (collision->separation.y != 0.0f) {
			collision->separation.y *= (projection + boxData1->HalfDimensions.y - distance) * 0.5f;
		}
	}

	projection = boxData2->HalfDimensions.x * glm::abs(glm::dot(boxData2->XAxis, boxData1->ZAxis))
		+ boxData2->HalfDimensions.y * glm::abs(glm::dot(boxData2->YAxis, boxData1->ZAxis))
		+ boxData2->HalfDimensions.z * glm::abs(glm::dot(boxData2->ZAxis, boxData1->ZAxis));

	if ((projection + boxData1->HalfDimensions.z) <= glm::abs(glm::dot(relativePosition, boxData1->ZAxis))) {
		return nullptr;
	}

	if (separate) {
		if (collision->separation.z != 0.0f) {
			collision->separation.z *= (projection + boxData1->HalfDimensions.z - distance) * 0.5f;
		}
	}

	relativePosition *= -1;

	projection = boxData1->HalfDimensions.x * glm::abs(glm::dot(boxData1->XAxis, boxData2->XAxis))
		+ boxData1->HalfDimensions.y * glm::abs(glm::dot(boxData1->YAxis, boxData2->XAxis))
		+ boxData1->HalfDimensions.z * glm::abs(glm::dot(boxData1->ZAxis, boxData2->XAxis));

	if ((projection + boxData2->HalfDimensions.x) <= glm::abs(glm::dot(relativePosition, boxData2->XAxis))) {
		return nullptr;
	}

	projection = boxData1->HalfDimensions.x * glm::abs(glm::dot(boxData1->XAxis, boxData2->YAxis))
		+ boxData1->HalfDimensions.y * glm::abs(glm::dot(boxData1->YAxis, boxData2->YAxis))
		+ boxData1->HalfDimensions.z * glm::abs(glm::dot(boxData1->ZAxis, boxData2->YAxis));

	if ((projection + boxData2->HalfDimensions.y) <= glm::abs(glm::dot(relativePosition, boxData2->YAxis))) {
		return nullptr;
	}

	projection = boxData1->HalfDimensions.x * glm::abs(glm::dot(boxData1->XAxis, boxData2->ZAxis))
		+ boxData1->HalfDimensions.y * glm::abs(glm::dot(boxData1->YAxis, boxData2->ZAxis))
		+ boxData1->HalfDimensions.z * glm::abs(glm::dot(boxData1->ZAxis, boxData2->ZAxis));

	if ((projection + boxData2->HalfDimensions.z) <= glm::abs(glm::dot(relativePosition, boxData2->ZAxis))) {
		return nullptr;
	}


	collision->collider = box1;
	collision->otherCollider = box2;
	//collision->position = ;

	if (separate) {
		collision->separation = glm::vec3(glm::dot(collision->separation, boxData1->XAxis),
			glm::dot(collision->separation, boxData1->YAxis),
			glm::dot(collision->separation, boxData1->ZAxis));
	}

	return collision;
}

Collision* Collider::CapsuleCapsuleCollision(Collider* capsule1, Collider* capsule2, bool separate)
{
	CapsuleColliderData* capsuleData1 = (CapsuleColliderData*)capsule1->shapeColliderData;
	CapsuleColliderData* capsuleData2 = (CapsuleColliderData*)capsule2->shapeColliderData;

	glm::vec3 S1 = capsuleData1->EndPosition - capsuleData1->Position;
	glm::vec3 S2 = capsuleData2->EndPosition - capsuleData2->Position;
	glm::vec3 r = capsuleData1->Position - capsuleData2->Position;
	float a = glm::dot(S1, S1);
	float e = glm::dot(S2, S2);
	float f = glm::dot(S2, r);

	float distanceSqr;
	glm::vec3 p1;
	glm::vec3 p2;

	if (a <= std::numeric_limits<float>::epsilon() && e <= std::numeric_limits<float>::epsilon()) {
		// Both segments degenerate into points
		distanceSqr = glm::dot(r, r);
		p1 = capsuleData1->Position;
		p2 = capsuleData2->Position;
	}
	else if (a <= std::numeric_limits<float>::epsilon()) {
		// First segment degenerates into a point
		distanceSqr = glm::dot(r, r) - f * f / e;
		p1 = capsuleData1->Position;

		float b = glm::dot(S1, S2);
		float c = glm::dot(S1, r);
		float t = (a * f - b * c) / (a * e - b * b);
		if (t < 0) {
			t = 0;
		}
		else if (t > 1) {
			t = 1;
		}

		p2 = capsuleData2->Position + S2 * t;
	}
	else if (e <= std::numeric_limits<float>::epsilon()) {
		// Second segment degenerates into a point
		distanceSqr = glm::dot(r, r) - glm::dot(S1, r) * glm::dot(S1, r) / a;
		float b = glm::dot(S1, S2);
		float c = glm::dot(S1, r);
		float s = (b * f - c * e) / (a * e - b * b);
		if (s < 0) {
			s = 0;
		}
		else if (s > 1) {
			s = 1;
		}

		p1 = capsuleData1->Position + S1 * s;
		p2 = capsuleData2->Position;
	}
	else {
		float b = glm::dot(S1, S2);
		float c = glm::dot(S1, r);
		float s = (b * f - c * e) / (a * e - b * b);
		float t = (a * f - b * c) / (a * e - b * b);

		if (s < 0) {
			s = 0;
		}
		else if (s > 1) {
			s = 1;
		}
		if (t < 0) {
			t = 0;
		}
		else if (t > 1) {
			t = 1;
		}

		p1 = capsuleData1->Position + S1 * s;
		p2 = capsuleData2->Position + S2 * t;
		//glm::vec3 closestPoint = (capsuleData1->Position - capsuleData2->Position) - S2 * t - S1 * s;
		glm::vec3 closestPoint = p1 - p2;
		distanceSqr = glm::dot(closestPoint, closestPoint);
	}


	float radiusSum = capsuleData1->Radius + capsuleData2->Radius;

	if (distanceSqr <= (radiusSum * radiusSum)) {
		Collision* collision = new Collision;
		collision->collider = capsule1;
		collision->otherCollider = capsule2;
		//collision->position = (capsuleData1->Radius * p1 + capsuleData2->Radius * p2) / radiusSum;

		if (separate) {
			collision->separation = glm::normalize(p1 - p2) * ((radiusSum - glm::sqrt(distanceSqr)) / 2.0f);
		}

		return collision;
	}
	else {
		return nullptr;
	}
}

Collision* Collider::SphereBoxCollision(Collider* sphere, Collider* box, bool separate)
{
	SphereColliderData* sphereData = (SphereColliderData*)sphere->shapeColliderData;
	BoxColliderData* boxData = (BoxColliderData*)box->shapeColliderData;

	glm::vec3 relativePosition = sphereData->Position - boxData->Position;
	relativePosition = glm::vec3(glm::dot(relativePosition, boxData->XAxis), glm::dot(relativePosition, boxData->YAxis),
		glm::dot(relativePosition, boxData->ZAxis));

	glm::vec3 closestPoint = glm::vec3(glm::clamp(relativePosition.x, -boxData->HalfDimensions.x, boxData->HalfDimensions.x),
		glm::clamp(relativePosition.y, -boxData->HalfDimensions.y, boxData->HalfDimensions.y),
		glm::clamp(relativePosition.z, -boxData->HalfDimensions.z, boxData->HalfDimensions.z));

	float distanceSqr = glm::dot(closestPoint - relativePosition, closestPoint - relativePosition);
	if (distanceSqr < (sphereData->Radius * sphereData->Radius)) {
		Collision* collision = new Collision;
		collision->collider = sphere;
		collision->otherCollider = box;
		//collision->position = closestPoint;

		if (separate) {
			//used to calculation of separation vector
			relativePosition = glm::vec3(0.0f, 0.0f, 0.0f);
			if (closestPoint.x >= boxData->HalfDimensions.x) {
				relativePosition.x = 1.0f;
			}
			else if (closestPoint.x <= (-boxData->HalfDimensions.x)) {
				relativePosition.x = -1.0f;
			}
			if (closestPoint.y >= boxData->HalfDimensions.y) {
				relativePosition.y = 1.0f;
			}
			else if (closestPoint.y <= (-boxData->HalfDimensions.y)) {
				relativePosition.y = -1.0f;
			}
			if (closestPoint.z >= boxData->HalfDimensions.z) {
				relativePosition.z = 1.0f;
			}
			else if (closestPoint.z <= (-boxData->HalfDimensions.z)) {
				relativePosition.z = -1.0f;
			}

			if ((relativePosition.x == 0.0f) && (relativePosition.y == 0.0f) && (relativePosition.z == 0.0f)) {
				float x = glm::abs(closestPoint.x);
				float y = glm::abs(closestPoint.y);
				float z = glm::abs(closestPoint.z);
				float d = 0.0f;

				if (x > y && x > z) {
					d = boxData->HalfDimensions.x - x;
					if (closestPoint.x > 0.0f) {
						relativePosition.x = 1.0f;
					}
					else {
						relativePosition.x = -1.0f;
					}
				}
				else if (y > x && y > z) {
					d = boxData->HalfDimensions.y - y;
					if (closestPoint.y > 0.0f) {
						relativePosition.y = 1.0f;
					}
					else {
						relativePosition.y = -1.0f;
					}
				}
				else {
					d = boxData->HalfDimensions.z - z;
					if (closestPoint.z > 0.0f) {
						relativePosition.z = 1.0f;
					}
					else {
						relativePosition.z = -1.0f;
					}
				}
				collision->separation = relativePosition * ((sphereData->Radius + d) / 2.0f);
			}
			else {
				collision->separation = glm::normalize(relativePosition) * ((sphereData->Radius - glm::sqrt(distanceSqr)) / 2.0f);
			}

			collision->separation = glm::vec3(glm::dot(collision->separation, { 1.0f, 0.0f, 0.0f }),
				glm::dot(collision->separation, { 0.0f, 1.0f, 0.0f }),
				glm::dot(collision->separation, { 0.0f, 0.0f, 1.0f }));
		}

		return collision;
	}
	else {
		return nullptr;
	}
}

Collision* Collider::SphereCapsuleCollision(Collider* sphere, Collider* capsule, bool separate)
{
	SphereColliderData* sphereData = (SphereColliderData*)sphere->shapeColliderData;
	CapsuleColliderData* capsuleData = (CapsuleColliderData*)capsule->shapeColliderData;

	glm::vec3 relativePosition = sphereData->Position - capsuleData->Position;
	glm::vec3 capsuleLength = capsuleData->EndPosition - capsuleData->Position;

	float s = glm::dot(relativePosition, capsuleLength) / glm::dot(capsuleLength, capsuleLength);

	if (s < 0) {
		s = 0;
	}
	else if (s > 1) {
		s = 1;
	}

	glm::vec3 closestPoint = capsuleData->Position + s * capsuleLength;
	s = capsuleData->Radius + sphereData->Radius;
	relativePosition = sphereData->Position - closestPoint;

	if (glm::dot(relativePosition, relativePosition) <= s * s) {
		Collision* collision = new Collision;
		collision->collider = sphere;
		collision->otherCollider = capsule;
		//collision->position = (sphereData->Position * sphereData->Radius + closestPoint * capsuleData->Radius) / s;

		if (separate) {
			collision->separation = glm::normalize(relativePosition) * ((s - relativePosition.length()) / 2.0f);
		}

		return collision;
	}
	else {
		return nullptr;
	}
}

Collision* Collider::BoxCapsuleCollision(Collider* box, Collider* capsule, bool separate)
{
	BoxColliderData* boxData = (BoxColliderData*)box->shapeColliderData;
	CapsuleColliderData* capsuleData = (CapsuleColliderData*)capsule->shapeColliderData;

	glm::vec3 dif = boxData->Position - capsuleData->Position;
	glm::vec3 seg = capsuleData->EndPosition - capsuleData->Position;
	float t = glm::dot(dif, seg);

	glm::vec3 pos;

	if (t < 0.0f) {
		pos = capsuleData->Position;
	}
	else if ((t * t / glm::dot(seg, seg)) > 1.0f) {
		pos = capsuleData->EndPosition;
	}
	else {
		pos = capsuleData->Position + glm::normalize(seg) * t;
	}

	dif = pos - boxData->Position;
	float projection = boxData->HalfDimensions.x * glm::abs(glm::dot(boxData->XAxis, dif)) +
		boxData->HalfDimensions.y * glm::abs(glm::dot(boxData->YAxis, dif)) +
		boxData->HalfDimensions.z * glm::abs(glm::dot(boxData->ZAxis, dif));

	if (((projection + capsuleData->Radius) * (projection + capsuleData->Radius)) > glm::dot(dif, dif)) {
		Collision* collision = new Collision;
		collision->collider = box;
		collision->otherCollider = capsule;
		//collision->position = ;

		if (separate) {
			collision->separation = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		return collision;
	}
	else {
		return nullptr;
	}
}


Collision* Collider::testCollision(Collider* collider, Collider* otherCollider, bool separate) {
	Collision* collision = nullptr;
	Collider* t;

	switch (collider->colliderShape) {
	case ColliderShape::SPHERE:
		switch (otherCollider->colliderShape)
		{
		case ColliderShape::SPHERE:
			return SphereSphereCollision(collider, otherCollider, separate);
			break;

		case ColliderShape::BOX:
			return SphereBoxCollision(collider, otherCollider, separate);
			break;

		case ColliderShape::CAPSULE:
			return SphereCapsuleCollision(collider, otherCollider, separate);
			break;
		}
		break;

	case ColliderShape::BOX:
		switch (otherCollider->colliderShape)
		{
		case ColliderShape::SPHERE:
			collision = SphereBoxCollision(otherCollider, collider, separate);
			if (collision != nullptr) {
				t = collision->collider;
				collision->collider = collision->otherCollider;
				collision->otherCollider = t;
			}
			break;

		case ColliderShape::BOX:
			return BoxBoxCollision(collider, otherCollider, separate);
			break;

		case ColliderShape::CAPSULE:
			return BoxCapsuleCollision(collider, otherCollider, separate);
			break;
		}
		break;

	case ColliderShape::CAPSULE:
		switch (otherCollider->colliderShape)
		{
		case ColliderShape::SPHERE:
			collision = SphereCapsuleCollision(otherCollider, collider, separate);
			if (collision != nullptr) {
				t = collision->collider;
				collision->collider = collision->otherCollider;
				collision->otherCollider = t;
			}
			break;

		case ColliderShape::BOX:
			collision = BoxCapsuleCollision(otherCollider, collider, separate);
			if (collision != nullptr) {
				t = collision->collider;
				collision->collider = collision->otherCollider;
				collision->otherCollider = t;
			}
			break;

		case ColliderShape::CAPSULE:
			return CapsuleCapsuleCollision(collider, otherCollider, separate);
			break;
		}
		break;
	}

	return collision;
}/**/
