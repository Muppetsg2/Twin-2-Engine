#include <core/CameraComponent.h>
#include <core/Transform.h>

Twin2Engine::Core::CameraType Twin2Engine::Core::CameraComponent::GetCameraType()
{
	return _type;
}

float Twin2Engine::Core::CameraComponent::GetNearPlane() const
{
	return _near;
}

float Twin2Engine::Core::CameraComponent::GetFarPlane() const
{
	return _far;
}

float Twin2Engine::Core::CameraComponent::GetFOV() const
{
	return _fov;
}

vec3 Twin2Engine::Core::CameraComponent::GetFrontDir() const
{
	return _front;
}

vec3 Twin2Engine::Core::CameraComponent::GetWorldUp() const
{
	return _worldUp;
}

vec3 Twin2Engine::Core::CameraComponent::GetRight() const
{
	return _right;
}

mat4 Twin2Engine::Core::CameraComponent::GetViewMatrix() const
{
	vec3 pos = GetTransform()->GetGlobalPosition();
	return lookAt(pos, pos + _front, _up);
}

mat4 Twin2Engine::Core::CameraComponent::GetProjectionMatrix()
{
	switch (_type) {
		case ORTHOGRAPHIC: {
			return ortho(0.f, (float)_w_width, 0.f, (float)_w_height, _near, _far);
			break;
		}
		case PERSPECTIVE: {
			return perspective(radians(_fov), (float)_w_width / (float)_w_height, _near, _far);
			break;
		}
	}
}

Frustum Twin2Engine::Core::CameraComponent::GetFrustum()
{
	Frustum frustum;
	float halfVSide = _far * tanf(_fov * .5f);
	float aspect = (float)_w_width / (float)_w_height;
	float halfHSide = halfVSide * aspect;
	vec3 frontMultFar = _far * _front;

	vec3 pos = GetTransform()->GetGlobalPosition();

	frustum.nearFace = { pos + _near * _front, _front };
	frustum.farFace = { pos + frontMultFar, -_front };
	frustum.rightFace = { pos, cross(frontMultFar - _right * halfHSide, _up) };
	frustum.leftFace = { pos, cross(_up, frontMultFar + _right * halfHSide) };
	frustum.topFace = { pos, cross(_right, frontMultFar - _up * halfVSide) };
	frustum.bottomFace = { pos, cross(frontMultFar + _up * halfVSide, _right) };

	return frustum;
}

bool Twin2Engine::Core::CameraComponent::IsMain() const
{
	return _isMain;
}

void Twin2Engine::Core::CameraComponent::SetWindowSize(vec2 size)
{
	_w_width = size.x;
	_w_height = size.y;
}

void Twin2Engine::Core::CameraComponent::SetFOV(float angle)
{
	_fov = angle;
}

void Twin2Engine::Core::CameraComponent::SetFarPlane(float value)
{
	_far = value;
}

void Twin2Engine::Core::CameraComponent::SetNearPlane(float value)
{
	_near = value;
}

void Twin2Engine::Core::CameraComponent::SetCameraType(Twin2Engine::Core::CameraType value)
{
	_type = value;
}

void Twin2Engine::Core::CameraComponent::SetFrontDir(vec3 dir)
{
	_front = normalize(dir);
	_right = normalize(cross(_front, _worldUp));
	_up = normalize(cross(_right, _front));
}

void Twin2Engine::Core::CameraComponent::SetWorldUp(vec3 value)
{
	_worldUp = normalize(value);
	_right = normalize(cross(_front, _worldUp));
	_up = normalize(cross(_right, _front));
}

void Twin2Engine::Core::CameraComponent::SetIsMain(bool value)
{
	_isMain = value;
}