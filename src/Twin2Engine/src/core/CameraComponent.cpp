#include <core/CameraComponent.h>
#include <core/Transform.h>
#include <core/YamlConverters.h>
#include <graphic/Window.h>
#include <GraphicEnigineManager.h>
#include <graphic/manager/ModelsManager.h>
#include <LightingController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

std::vector<CameraComponent*> CameraComponent::Cameras = std::vector<CameraComponent*>();
GLuint CameraComponent::_uboMatrices = 0;
GLuint CameraComponent::_uboWindowData = 0;
InstatiatingModel CameraComponent::_renderPlane = InstatiatingModel();
Shader* CameraComponent::_renderShader = nullptr;

void CameraComponent::OnTransformChange(Transform* trans)
{
	glm::vec3 rot = trans->GetGlobalRotation();

	glm::vec3 front{};
	front.x = cos(glm::radians(rot.y)) * cos(glm::radians(rot.x));
	front.y = sin(glm::radians(rot.x));
	front.z = sin(glm::radians(rot.y)) * cos(glm::radians(rot.x));
	this->SetFrontDir(glm::normalize(front));

	/*
	if (this->_isMain) {
		glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(this->GetViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	*/
}

void CameraComponent::OnWindowSizeChange()
{
	ivec2 wSize = Window::GetInstance()->GetContentSize();

	unsigned int r_res = GL_RGB;
	unsigned int d_res = GL_DEPTH_COMPONENT;

	switch (_renderRes) {
		case DEFAULT: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			break;
		}
		case MEDIUM: {
			r_res = GL_RGB16F;
			d_res = GL_DEPTH_COMPONENT16;
			break;
		}
		case HIGH: {
			r_res = GL_RGB32F;
			d_res = GL_DEPTH_COMPONENT32F;
			break;
		}
		default: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, d_res, wSize.x, wSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, _renderMap);
	glTexImage2D(GL_TEXTURE_2D, 0, r_res, wSize.x, wSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _msRenderMap);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, r_res, wSize.x, wSize.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, _msRenderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_DEPTH24_STENCIL8, wSize.x, wSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

CameraType CameraComponent::GetCameraType() const
{
	return _type;
}

uint8_t CameraComponent::GetCameraFilters() const
{
	return _filters;
}

uint8_t CameraComponent::GetSamples() const
{
	return _samples;
}

RenderResolution CameraComponent::GetRenderResolution() const
{
	return _renderRes;
}

float CameraComponent::GetFOV() const
{
	return _fov;
}

float CameraComponent::GetGamma() const
{
	return _gamma;
}

float CameraComponent::GetNearPlane() const
{
	return _near;
}

float CameraComponent::GetFarPlane() const
{
	return _far;
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

mat4 CameraComponent::GetProjectionMatrix() const
{
	ivec2 size = Window::GetInstance()->GetContentSize();

	switch (_type) {
		case ORTHOGRAPHIC: {
			return ortho(0.f, (float)size.x * 0.005f, 0.f, (float)size.y * 0.005f, _near, _far);
			break;
		}
		case PERSPECTIVE: {
			return perspective(radians(_fov), (size.y != 0) ? ((float)size.x / (float)size.y) : 0, _near, _far);
			break;
		}
	}
	return mat4(1.f);
}

Frustum CameraComponent::GetFrustum() const
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

void CameraComponent::SetGamma(float gamma)
{
	_gamma = gamma;
}

void CameraComponent::SetFarPlane(float value)
{
	_far = value;
}

void CameraComponent::SetNearPlane(float value)
{
	_near = value;
}

void CameraComponent::SetCameraFilter(uint8_t filters)
{
	_filters = filters;
}

void CameraComponent::SetCameraType(CameraType value)
{
	_type = value;

	/*
	if (this->IsMain()) {
		glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(this->GetProjectionMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	*/
}

void CameraComponent::SetSamples(uint8_t i)
{
	_samples = i > 16 ? 16 : i < 1 ? 1 : i;

	if (_isInit) {
		ivec2 wSize = Window::GetInstance()->GetContentSize();

		unsigned int r_res = GL_RGB;

		switch (_renderRes) {
			case DEFAULT: {
				r_res = GL_RGB;
				break;
			}
			case MEDIUM: {
				r_res = GL_RGB16F;
				break;
			}
			case HIGH: {
				r_res = GL_RGB32F;
				break;
			}
			default: {
				r_res = GL_RGB;
				break;
			}
		}

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _msRenderMap);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, r_res, wSize.x, wSize.y, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, _msRenderBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_DEPTH24_STENCIL8, wSize.x, wSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}

void CameraComponent::SetRenderResolution(RenderResolution res)
{
	_renderRes = res;

	if (_isInit) {
		ivec2 wSize = Window::GetInstance()->GetContentSize();

		unsigned int r_res = GL_RGB;
		unsigned int d_res = GL_DEPTH_COMPONENT;

		switch (_renderRes) {
			case DEFAULT: {
				r_res = GL_RGB;
				d_res = GL_DEPTH_COMPONENT;
				break;
			}
			case MEDIUM: {
				r_res = GL_RGB16F;
				d_res = GL_DEPTH_COMPONENT16;
				break;
			}
			case HIGH: {
				r_res = GL_RGB32F;
				d_res = GL_DEPTH_COMPONENT32F;
				break;
			}
			default: {
				r_res = GL_RGB;
				d_res = GL_DEPTH_COMPONENT;
				break;
			}
		}

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _msRenderMap);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, r_res, wSize.x, wSize.y, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glBindTexture(GL_TEXTURE_2D, _depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, d_res, wSize.x, wSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, _renderMap);
		glTexImage2D(GL_TEXTURE_2D, 0, r_res, wSize.x, wSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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
			if (c != this) {
				c->SetIsMain(false);
			}
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

void CameraComponent::Render()
{
	glm::vec3 clear_color = glm::vec3(powf(.1f, _gamma));
	// UBO's
	//Jesli wiecej kamer i kazda ma ze swojego kata dawac obraz

	// DEFAULT
	
	glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(this->GetProjectionMatrix()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(this->GetViewMatrix()));

	glBindBuffer(GL_UNIFORM_BUFFER, _uboWindowData);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec2), value_ptr(Window::GetInstance()->GetContentSize()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2), sizeof(float), &(this->_near));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2) + sizeof(float), sizeof(float), &(this->_far));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2) + sizeof(float) * 2, sizeof(float), &(this->_gamma));
	
	
	// NAMED
	/*glNamedBufferSubData(_uboMatrices, 0, sizeof(mat4), value_ptr(this->GetProjectionMatrix()));
	glNamedBufferSubData(_uboMatrices, sizeof(mat4), sizeof(mat4), value_ptr(this->GetViewMatrix()));

	glNamedBufferSubData(_uboWindowData, 0, sizeof(vec2), value_ptr(Window::GetInstance()->GetContentSize()));
	glNamedBufferSubData(_uboWindowData, sizeof(vec2), sizeof(float), &(this->_near));
	glNamedBufferSubData(_uboWindowData, sizeof(vec2) + sizeof(float), sizeof(float), &(this->_far));
	glNamedBufferSubData(_uboWindowData, sizeof(vec2) + sizeof(float) * 2, sizeof(float), &(this->_gamma));*/

	// DEPTH MAP
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GraphicEngineManager::DepthRender();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ivec2 wSize = Window::GetInstance()->GetContentSize();


	LightingSystem::LightingController::Instance()->RenderShadowMaps();

	// RENDER MAP
	glBindFramebuffer(GL_FRAMEBUFFER, _msRenderMapFBO);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GraphicEngineManager::Render();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, _msRenderMapFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderMapFBO);
	glBlitFramebuffer(0, 0, wSize.x, wSize.y, 0, 0, wSize.x, wSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// RENDERING
	if (this->IsMain()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		BindRenderTexture(0);
		BindDepthTexture(1);
		_renderShader->Use();
		_renderShader->SetInt("screenTexture", 0);
		_renderShader->SetInt("depthTexture", 1);

		_renderShader->SetBool("hasBlur", (_filters & RenderFilter::BLUR) != 0);
		_renderShader->SetBool("hasVignette", (_filters & RenderFilter::VIGNETTE) != 0);
		_renderShader->SetBool("hasNegative", (_filters & RenderFilter::NEGATIVE) != 0);
		_renderShader->SetBool("hasGrayscale", (_filters & RenderFilter::GRAYSCALE) != 0);
		_renderShader->SetBool("displayDepth", (_filters & RenderFilter::DEPTH) != 0);

		_renderPlane.GetMesh(0)->Draw(1);
	}
}

void CameraComponent::BindRenderTexture(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, _renderMap);
}

void CameraComponent::BindDepthTexture(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
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

		glGenBuffers(1, &_uboWindowData);

		glBindBuffer(GL_UNIFORM_BUFFER, _uboWindowData);
		glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(float) + sizeof(vec2), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 1, _uboWindowData, 0, 3 * sizeof(float) + sizeof(vec2));

		glBindBuffer(GL_UNIFORM_BUFFER, _uboWindowData);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec2), value_ptr(Window::GetInstance()->GetContentSize()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2), sizeof(float), &(this->_near));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2) + sizeof(float), sizeof(float), &(this->_far));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2) + sizeof(float) * 2, sizeof(float), &(this->_gamma));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		_renderPlane = ModelsManager::GetPlane();
		_renderShader = ShaderManager::CreateShaderProgram("CameraPlaneShader", "/shaders/screen.vert", "/shaders/screen.frag");
	}

	this->_camId = Cameras.size();
	Cameras.push_back(this);

	_transformEventId = GetTransform()->OnEventTransformChanged += [&](Transform* t) -> void { OnTransformChange(t); };
	_windowEventId = Window::GetInstance()->OnWindowSizeEvent += [&]() -> void { OnWindowSizeChange(); };

	ivec2 wSize = Window::GetInstance()->GetContentSize();

	unsigned int r_res = GL_RGB;
	unsigned int d_res = GL_DEPTH_COMPONENT;

	switch (_renderRes) {
		case DEFAULT: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			break;
		}
		case MEDIUM: {
			r_res = GL_RGB16F;
			d_res = GL_DEPTH_COMPONENT16;
			break;
		}
		case HIGH: {
			r_res = GL_RGB32F;
			d_res = GL_DEPTH_COMPONENT32F;
			break;
		}
		default: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			break;
		}
	}

#pragma region DepthBuffer

	glGenFramebuffers(1, &_depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);

	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, d_res, wSize.x, wSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

#pragma endregion

#pragma region RenderBuffer

	// MSAA Render Buffer
	glGenFramebuffers(1, &_msRenderMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _msRenderMapFBO);

	glGenTextures(1, &_msRenderMap);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _msRenderMap);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, r_res, wSize.x, wSize.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _msRenderMap, 0);

	glGenRenderbuffers(1, &_msRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _msRenderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_DEPTH24_STENCIL8, wSize.x, wSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _msRenderBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Intermidiate Render FBO
	glGenFramebuffers(1, &_renderMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _renderMapFBO);

	// Color Attachment Texture
	glGenTextures(1, &_renderMap);
	glBindTexture(GL_TEXTURE_2D, _renderMap);

	glTexImage2D(GL_TEXTURE_2D, 0, r_res, wSize.x, wSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderMap, 0);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

#pragma endregion

	_isInit = true;
}

void CameraComponent::OnDestroy()
{
	// Depth Map
	glDeleteTextures(1, &_depthMap);
	glDeleteFramebuffers(1, &_depthMapFBO);

	// Render Map
	glDeleteTextures(1, &_renderMap);
	glDeleteTextures(1, &_msRenderMap);
	glDeleteFramebuffers(1, &_renderMapFBO);
	glDeleteFramebuffers(1, &_msRenderMapFBO);
	glDeleteRenderbuffers(1, &_msRenderBuffer);

	GetTransform()->OnEventTransformChanged -= _transformEventId;
	Window::GetInstance()->OnWindowSizeEvent -= _windowEventId;

	Cameras.erase(Cameras.begin() + this->_camId);

	if (Cameras.size() == 0) {
		glDeleteBuffers(1, &_uboMatrices);
		glDeleteBuffers(1, &_uboWindowData);
	}
}

CollisionSystem::Ray CameraComponent::GetScreenPointRay(glm::vec2 screenPosition)
{
	ivec2 size = Window::GetInstance()->GetContentSize();
	glm::vec3 Origin = GetTransform()->GetGlobalPosition();

	/*/glm::vec4 Position = glm::inverse(GetProjectionMatrix() * GetViewMatrix()) * glm::vec4(2.0f * screenPosition.x / size.x - 1.0f,
																						   2.0f * screenPosition.y / size.y - 1.0f, 1.0f, 1.0f);
	//SPDLOG_INFO("P: {}, {}, {}, {}", Position.x, Position.y, Position.z, Position.w);
	//float fov_tan = glm::tan(3.141 * _fov / 180.0f);
	

	glm::vec3 Direction = glm::normalize(glm::vec3(Position) - Origin);
	//glm::vec3 Direction = glm::normalize(glm::vec3((2.0f * screenPosition.x / size.x - 1.0f) * fov_tan,
	//											   (1.0f - 2.0f * screenPosition.y / size.y) * fov_tan, 1.0f));/**/

	// Normalize screen coordinates to NDC (-1 to 1)
	float x = (2.0f * screenPosition.x) / size.x - 1.0f;
	float y = 1.0f - (2.0f * screenPosition.y) / size.y;
	float z = 1.0f; // NDC Z value (should be 1 for far plane)

	// Unproject to view space
	glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(GetProjectionMatrix()) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Set z to -1 for direction
	glm::vec4 rayWorld = glm::inverse(GetViewMatrix()) * rayEye;

	// Get direction vector in world space
	glm::vec3 Direction(rayWorld);
	Direction = glm::normalize(Direction);

	return CollisionSystem::Ray(std::move(Direction), std::move(Origin));
}

YAML::Node CameraComponent::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Camera";
	node.remove("subTypes");
	node["fov"] = _fov;
	node["nearPlane"] = _near;
	node["farPlane"] = _far;
	node["cameraFilter"] = (size_t)_filters;
	node["cameraType"] = _type;
	node["samples"] = (size_t)_samples;
	node["renderRes"] = _renderRes;
	node["gamma"] = _gamma;
	node["frontDir"] = _front;
	node["worldUp"] = _worldUp;
	node["isMain"] = _isMain;
	return node;
}
