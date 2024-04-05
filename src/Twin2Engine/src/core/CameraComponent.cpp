#include <core/CameraComponent.h>
#include <core/Transform.h>
#include <graphic/Window.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;

std::vector<CameraComponent*> CameraComponent::Cameras = std::vector<CameraComponent*>();
GLuint CameraComponent::_uboMatrices = 0;

void CameraComponent::OnTransformChange(Transform* trans)
{
	printf("%d\n", this->IsMain());

	if (this->_isMain) {
		glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(this->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

CameraType CameraComponent::GetCameraType()
{
	return _type;
}

float CameraComponent::GetNearPlane() const
{
	return _near;
}

float CameraComponent::GetFarPlane() const
{
	return _far;
}

float CameraComponent::GetFOV() const
{
	return _fov;
}

vec3 CameraComponent::GetFrontDir() const
{
	return _front;
}

vec3 CameraComponent::GetWorldUp() const
{
	return _worldUp;
}

vec3 CameraComponent::GetRight() const
{
	return _right;
}

mat4 CameraComponent::GetViewMatrix() const
{
	vec3 pos = GetTransform()->GetGlobalPosition();
	return lookAt(pos, pos + _front, _up);
}

mat4 CameraComponent::GetProjectionMatrix()
{
	ivec2 size = Window::GetInstance()->GetContentSize();

	switch (_type) {
		case ORTHOGRAPHIC: {
			return ortho(0.f, (float)size.x, 0.f, (float)size.y, _near, _far);
			break;
		}
		case PERSPECTIVE: {
			return perspective(radians(_fov), (float)size.x / (float)size.y, _near, _far);
			break;
		}
	}
}

Frustum CameraComponent::GetFrustum()
{
	ivec2 size = Window::GetInstance()->GetContentSize();

	Frustum frustum;
	float halfVSide = _far * tanf(_fov * .5f);
	float aspect = (float)size.x / (float)size.y;
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

bool CameraComponent::IsMain() const
{
	return _isMain;
}

void CameraComponent::SetFOV(float angle)
{
	_fov = angle;
}

void CameraComponent::SetFarPlane(float value)
{
	_far = value;
}

void CameraComponent::SetNearPlane(float value)
{
	_near = value;
}

void CameraComponent::SetCameraType(CameraType value)
{
	_type = value;
}

void CameraComponent::SetFrontDir(vec3 dir)
{
	_front = normalize(dir);
	_right = normalize(cross(_front, _worldUp));
	_up = normalize(cross(_right, _front));
}

void CameraComponent::SetWorldUp(vec3 value)
{
	_worldUp = normalize(value);
	_right = normalize(cross(_front, _worldUp));
	_up = normalize(cross(_right, _front));
}

void CameraComponent::SetIsMain(bool value)
{
	if (value) {
		for (auto c : Cameras) {
			c->SetIsMain(false);
		}
	}
	else if (!value && this->_isMain) {
		if (this->_camId == 0 && Cameras.size() > 1) {
			Cameras[1]->SetIsMain(true);
		}
		else {
			Cameras[0]->SetIsMain(true);
		}
	}
	_isMain = value;
}

void CameraComponent::StartRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _renderMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CameraComponent::StartDepthTest()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CameraComponent::EndRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CameraComponent::EndDepthTest()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint CameraComponent::GetRenderTexture()
{
	return _renderMap;
}

GLuint CameraComponent::GetDepthTexture()
{
	return _depthMap;
}

CameraComponent* CameraComponent::GetMainCamera()
{
	for (CameraComponent* c : Cameras) {
		if (c->IsMain()) {
			return c;
		}
	}

	return nullptr;
}


void CameraComponent::Initialize()
{
	if (Cameras.size() == 0) {

		this->SetIsMain(true);

		glGenBuffers(1, &_uboMatrices);

		glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, _uboMatrices, 0, 2 * sizeof(mat4));

		glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(this->GetProjectionMatrix()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(this->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	this->_camId = Cameras.size();
	Cameras.push_back(this);
	GetTransform()->OnEventPositionChanged += [&](Transform* t) -> void { OnTransformChange(t); };
	GetTransform()->OnEventRotationChanged += [&](Transform* t) -> void { OnTransformChange(t); };

	ivec2 wSize = Window::GetInstance()->GetContentSize();

#pragma region DepthBuffer

	glGenFramebuffers(1, &_depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);

	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, wSize.x, wSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

#pragma endregion

#pragma region RenderBuffer

	glGenFramebuffers(1, &_renderMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _renderMapFBO);

	glGenTextures(1, &_renderMap);
	glBindTexture(GL_TEXTURE_2D, _renderMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wSize.x, wSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _renderMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

#pragma endregion
}

void CameraComponent::OnDestroy()
{
	// Depth Map
	glDeleteTextures(1, &_depthMap);
	glDeleteFramebuffers(1, &_depthMapFBO);

	// Render Map
	glDeleteTextures(1, &_renderMap);
	glDeleteFramebuffers(1, &_renderMapFBO);

	//GetTransform()->OnEventPositionChanged -= [&](Transform* t) -> void { OnTransformChange(t); };
	//GetTransform()->OnEventRotationChanged -= [&](Transform* t) -> void { OnTransformChange(t); };

	Cameras.erase(Cameras.begin() + this->_camId);

	if (Cameras.size() == 0) {
		glDeleteBuffers(1, &_uboMatrices);
	}
}