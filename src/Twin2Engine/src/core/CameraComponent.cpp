#include <core/CameraComponent.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>
#include <graphic/Window.h>
#include <graphic/LightingController.h>
#include <GraphicEnigine.h>
#include <graphic/manager/ModelsManager.h>
#include <core/MathExtensions.h>
#include <core/Time.h>
#include <ParticleSystemsController.h>

#if TRACY_PROFILER
const char* const tracy_RenderDepthBuffer = "RenderDepthBuffer";
const char* const tracy_RenderSSAOTexture = "RenderSSAOTexture";
const char* const tracy_BlurSSAOTexture = "BlurSSAOTexture";
const char* const tracy_UpdateRenderingQueues = "UpdateRenderingQueues";
const char* const tracy_RenderScreenTexture = "RenderScreenTexture";
const char* const tracy_OnScreenFramebuffer = "OnScreenFramebuffer";
#endif

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Tools;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

size_t TransformChangeEventId = 0;

std::vector<CameraComponent*> CameraComponent::Cameras = std::vector<CameraComponent*>();
GLuint CameraComponent::_uboCameraData = 0;
STD140Struct CameraComponent::_uboCameraDataStruct{
	STD140Variable<mat4>("projection"),
	STD140Variable<mat4>("view"),
	STD140Variable<vec3>("viewPos"),
	STD140Variable<bool>("isSSAO")
};
GLuint CameraComponent::_uboWindowData = 0;
STD140Struct CameraComponent::_uboWindowDataStruct{
		STD140Variable<ivec2>("windowSize"),
		STD140Variable<float>("nearPlane"),
		STD140Variable<float>("farPlane"),
		STD140Variable<float>("gamma"),
		STD140Variable<float>("time"),
		STD140Variable<float>("deltaTime")
};
GLuint CameraComponent::_screenPlaneVAO = 0;
GLuint CameraComponent::_screenPlaneVBO = 0;
GLuint CameraComponent::_screenPlaneEBO = 0;
float CameraComponent::_screenVerts[8] = {
	-1.f, 1.f,  //0.f, 0.f,
	1.f, 1.f,   //1.f, 0.f,
	-1.f, -1.f, //0.f, 1.f,
	1.f, -1.f   //, 1.f, 1.f
};
GLuint CameraComponent::_screenIndicies[6] = {
	2, 1, 0,
	2, 3, 1
};
Shader* CameraComponent::_screenShader = nullptr;
Shader* CameraComponent::_ssaoShader = nullptr;
Shader* CameraComponent::_ssaoBlurredShader = nullptr;
Shader* CameraComponent::_depthShader = nullptr;
Frustum CameraComponent::_currentCameraFrustum = Graphic::Frustum();

std::vector<glm::vec3> CameraComponent::_ssaoKernel = std::vector<glm::vec3>();
float* CameraComponent::_ssaoTextureData = nullptr;
GLuint CameraComponent::_ssaoNoiseTexture = NULL;

void CameraComponent::OnTransformChange(Transform* trans)
{
	UpdateFrontDir();
}

void CameraComponent::OnWindowSizeChange()
{
	ivec2 wSize = Window::GetInstance()->GetContentSize();

	unsigned int r_res = GL_RGB;
	unsigned int d_res = GL_DEPTH_COMPONENT;
	unsigned int s_res = GL_RED;

	switch (_renderRes) {
		case CameraRenderResolution::DEFAULT: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			s_res = GL_RED;
			break;
		}
		case CameraRenderResolution::MEDIUM: {
			r_res = GL_RGB16F;
			d_res = GL_DEPTH_COMPONENT16;
			s_res = GL_RED;
			break;
		}
		case CameraRenderResolution::HIGH: {
			r_res = GL_RGB32F;
			d_res = GL_DEPTH_COMPONENT32F;
			s_res = GL_R32F;
			break;
		}
		default: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			s_res = GL_RED;
			break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, d_res, wSize.x, wSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, _renderMap);
	glTexImage2D(GL_TEXTURE_2D, 0, r_res, wSize.x, wSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, _ssaoMap);
	glTexImage2D(GL_TEXTURE_2D, 0, s_res, wSize.x / 2, wSize.y / 2, 0, GL_RED, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, _ssaoBlurredMap);
	glTexImage2D(GL_TEXTURE_2D, 0, s_res, wSize.x, wSize.y, 0, GL_RED, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _msRenderMap);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, r_res, wSize.x, wSize.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, _msRenderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_DEPTH24_STENCIL8, wSize.x, wSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void CameraComponent::SetFrontDir(vec3 dir)
{
	_front = normalize(dir);
	_right = normalize(cross(_front, _worldUp));
	_up = normalize(cross(_right, _front));
}

void CameraComponent::GenerateSSAOKernel(unsigned int size)
{
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
	std::default_random_engine generator;
	for (int i = 0; i < size; ++i) {
		vec3 sample = vec3(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);

		// normalize sample
		sample = normalize(sample);
		float scale = (float)i / (float)size;
		scale = lerpf(0.1f, 1.0f, scale * scale);
		sample *= scale;
		_ssaoKernel.push_back(sample);
	}
}

void CameraComponent::GenerateSSAONoiseTexture()
{
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
	std::default_random_engine generator;
	_ssaoTextureData = new float[16 * 3];
	for (int i = 0; i < 16; i++)
	{
		vec3 sample = vec3(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0
		);
		_ssaoTextureData[i * 3 + 0] = sample[0];
		_ssaoTextureData[i * 3 + 1] = sample[1];
		_ssaoTextureData[i * 3 + 2] = sample[2];
	}

	glGenTextures(1, &_ssaoNoiseTexture);
	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, _ssaoTextureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CameraComponent::GenerateBlurKernel()
{
	float sigma = 7.0;
	int kSize = (_blurMSize - 1) / 2;
	for (int j = 0; j <= kSize; ++j)
	{
		_blurKernel[kSize + j] = _blurKernel[kSize - j] = normpdf(float(j), sigma);
	}
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

CameraRenderResolution CameraComponent::GetRenderResolution() const
{
	return _renderRes;
}

CameraDisplayMode CameraComponent::GetDisplayMode() const
{
	return _mode;
}

float CameraComponent::GetFOV() const
{
	return _fov;
}

float CameraComponent::GetGamma() const
{
	return _gamma;
}

float CameraComponent::GetBrightness() const
{
	return _brightness;
}

float CameraComponent::GetContrast() const
{
	return _contrast;
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
		case CameraType::ORTHOGRAPHIC: {
			return ortho(0.f, (float)size.x * 0.005f, 0.f, (float)size.y * 0.005f, _near, _far);
			break;
		}
		case CameraType::PERSPECTIVE: {
			return perspective(radians(_fov), (size.y != 0) ? ((float)size.x / (float)size.y) : 0, _near, _far);
			break;
		}
	}
	return mat4(1.f);
}

Frustum CameraComponent::GetFrustum() const
{
	ivec2 size = Window::GetInstance()->GetContentSize();

	Graphic::Frustum frustum;
	float halfVSide = _far * tanf(_fov * 0.5f);
	float aspect = (float)size.x / (float)size.y;
	float halfHSide = halfVSide * aspect;
	vec3 frontMultFar = _far * _front;

	vec3 pos = GetTransform()->GetGlobalPosition();

	frustum.nearFace = { pos + _near * _front, _front };
	frustum.farFace = { pos + frontMultFar, -_front };
	frustum.rightFace = { pos, glm::normalize(cross(_up, frontMultFar + _right * halfHSide)) };
	frustum.leftFace = { pos, glm::normalize(cross(frontMultFar - _right * halfHSide, _up)) };
	frustum.topFace = { pos, glm::normalize(cross(frontMultFar + _up * halfVSide, _right)) };
	frustum.bottomFace = { pos, glm::normalize(cross(_right, frontMultFar - _up * halfVSide)) };

	return frustum;
}


bool CameraComponent::IsMain() const
{
	return _isMain;
}

bool CameraComponent::IsFrustumCullingOn() const
{
	return _isFrustumCulling;
}

bool CameraComponent::IsSSAO() const
{
	return _isSsao;
}

void CameraComponent::SetFOV(float angle)
{
	_fov = angle;
}

void CameraComponent::SetGamma(float gamma)
{
	_gamma = gamma;
}

void CameraComponent::SetBrightness(float brightness)
{
	_brightness = brightness;
}

void CameraComponent::SetContrast(float contrast)
{
	_contrast = contrast;
}

void CameraComponent::SetFarPlane(float value)
{
	_far = value;
}

void CameraComponent::SetNearPlane(float value)
{
	_near = value;
}

void CameraComponent::SetBlurMatrixSize(size_t mSize) {
	if (mSize < 0) mSize = 0;
	else if (mSize > 40) mSize = 40;

	if (_blurMSize != mSize) {
		_blurMSize = mSize;
		if (_blurMSize != 0) {
			GenerateBlurKernel();
		}
	}
}

void CameraComponent::SetCameraFilter(uint8_t filters)
{
	_filters = filters;
}

void CameraComponent::SetCameraType(CameraType value)
{
	_type = value;
}

void CameraComponent::SetSamples(uint8_t i)
{
	_samples = i > 16 ? 16 : i < 1 ? 1 : i;

	if (_isInit) {
		ivec2 wSize = Window::GetInstance()->GetContentSize();

		unsigned int r_res = GL_RGB;

		switch (_renderRes) {
			case CameraRenderResolution::DEFAULT: {
				r_res = GL_RGB;
				break;
			}
			case CameraRenderResolution::MEDIUM: {
				r_res = GL_RGB16F;
				break;
			}
			case CameraRenderResolution::HIGH: {
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

void CameraComponent::SetRenderResolution(CameraRenderResolution res)
{
	_renderRes = res;

	if (_isInit) {
		ivec2 wSize = Window::GetInstance()->GetContentSize();

		unsigned int r_res = GL_RGB;
		unsigned int d_res = GL_DEPTH_COMPONENT;
		unsigned int s_res = GL_RED;

		switch (_renderRes) {
			case CameraRenderResolution::DEFAULT: {
				r_res = GL_RGB;
				d_res = GL_DEPTH_COMPONENT;
				s_res = GL_RED;
				break;
			}
			case CameraRenderResolution::MEDIUM: {
				r_res = GL_RGB16F;
				d_res = GL_DEPTH_COMPONENT16;
				s_res = GL_RED;
				break;
			}
			case CameraRenderResolution::HIGH: {
				r_res = GL_RGB32F;
				d_res = GL_DEPTH_COMPONENT32F;
				s_res = GL_R32F;
				break;
			}
			default: {
				r_res = GL_RGB;
				d_res = GL_DEPTH_COMPONENT;
				s_res = GL_RED;
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

		glBindTexture(GL_TEXTURE_2D, _ssaoMap);
		glTexImage2D(GL_TEXTURE_2D, 0, s_res, wSize.x / 2, wSize.y / 2, 0, GL_RED, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, _ssaoBlurredMap);
		glTexImage2D(GL_TEXTURE_2D, 0, s_res, wSize.x, wSize.y, 0, GL_RED, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CameraComponent::SetDisplayMode(CameraDisplayMode cdm)
{
	_mode = cdm;
}

void CameraComponent::SetWorldUp(vec3 value)
{
	_worldUp = normalize(value);
	_right = normalize(cross(_front, _worldUp));
	_up = normalize(cross(_right, _front));
}

void CameraComponent::UpdateFrontDir()
{
	glm::vec3 rot = GetTransform()->GetGlobalRotation();

	glm::vec3 front{};
	front.x = cos(glm::radians(rot.y)) * cos(glm::radians(rot.x));
	front.y = sin(glm::radians(rot.x));
	front.z = sin(glm::radians(rot.y)) * cos(glm::radians(rot.x));
	this->SetFrontDir(glm::normalize(front));
}

void CameraComponent::SetIsMain(bool value)
{
	if (value) {
		for (auto c : Cameras) {
			if (c != this) {
				c->GetTransform()->OnEventTransformChanged -= TransformChangeEventId;
				c->SetIsMain(false);
			}
		}
	}
	else if (!value && this->_isMain) {
		if (this->_camId == 0 && Cameras.size() > 1) {
			Cameras[1]->SetIsMain(true);
			TransformChangeEventId = Cameras[1]->GetTransform()->OnEventTransformChanged += [](Transform* transform) {
				LightingController::Instance()->UpdateOnTransformChange();
			};
		}
		else {
			Cameras[0]->SetIsMain(true);
			TransformChangeEventId = Cameras[0]->GetTransform()->OnEventTransformChanged += [](Transform* transform) {
				LightingController::Instance()->UpdateOnTransformChange();
			};
		}
	}
	_isMain = value;
}

void CameraComponent::SetFrustumCulling(bool value)
{
	_isFrustumCulling = value;
}

void CameraComponent::SetSSAO(bool value)
{
	_isSsao = value;
}

void CameraComponent::Render()
{
#if TRACY_PROFILER
	ZoneScoped;
#endif
	if (!IsEnable() || !GetGameObject()->GetActive()) {
		return;
	}

	if (_isFrustumCulling)
		_currentCameraFrustum = GetFrustum();

	vec3 clear_color = glm::vec3(powf(.1f, _gamma));
	ivec2 wSize = Window::GetInstance()->GetContentSize();

	// DEFAULT
	glBindBuffer(GL_UNIFORM_BUFFER, _uboCameraData);

	_uboCameraDataStruct.Set("projection", this->GetProjectionMatrix());
	_uboCameraDataStruct.Set("view", this->GetViewMatrix());
	_uboCameraDataStruct.Set("viewPos", this->GetTransform()->GetGlobalPosition());
	_uboCameraDataStruct.Set("isSSAO", _isSsao);

	//Jesli wiecej kamer i kazda ma ze swojego kata dawac obraz
	glBufferSubData(GL_UNIFORM_BUFFER, 0, _uboCameraDataStruct.GetSize(), _uboCameraDataStruct.GetData().data());

	glBindBuffer(GL_UNIFORM_BUFFER, _uboWindowData);

	_uboWindowDataStruct.Set("windowSize", wSize);
	_uboWindowDataStruct.Set("nearPlane", this->_near);
	_uboWindowDataStruct.Set("farPlane", this->_far);
	_uboWindowDataStruct.Set("gamma", this->_gamma);
	_uboWindowDataStruct.Set("time", Time::GetTime());
	_uboWindowDataStruct.Set("deltaTime", Time::GetDeltaTime());

	glBindBuffer(GL_UNIFORM_BUFFER, _uboWindowData);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, _uboWindowDataStruct.GetSize(), _uboWindowDataStruct.GetData().data());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	if (wSize.y != 0) {
		// UPDATING RENDERER
#if TRACY_PROFILER
		FrameMarkStart(tracy_UpdateRenderingQueues);
#endif

		GraphicEngine::UpdateBeforeRendering();
		GraphicEngine::RenderShadow();

#if TRACY_PROFILER
		FrameMarkEnd(tracy_UpdateRenderingQueues);
#endif

		glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, wSize.x, wSize.y);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#if TRACY_PROFILER
			FrameMarkStart(tracy_RenderDepthBuffer);
#endif

			_depthShader->Use();
			GraphicEngine::PreRender();

#if TRACY_PROFILER
			FrameMarkEnd(tracy_RenderDepthBuffer);
#endif

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		

		if (_isSsao && _mode != CameraDisplayMode::DEPTH) {
			// SSAO MAP
			glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoMap, 0);
			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, wSize.x / 2, wSize.y / 2);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#if TRACY_PROFILER
				FrameMarkStart(tracy_RenderSSAOTexture);
#endif

				_ssaoShader->Use();
				BindDepthTexture(0);
				_ssaoShader->SetInt("depthTexture", 0);
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTexture);
				_ssaoShader->SetInt("noiseTexture", 1);
				_ssaoShader->SetFloat("sampleRadius", _ssaoSampleRadius);
				_ssaoShader->SetFloat("bias", _ssaoBias);

				glBindVertexArray(_screenPlaneVAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &_screenIndicies);
				glBindVertexArray(0);

				glBindTexture(GL_TEXTURE_2D, 0);
				
#if TRACY_PROFILER
				FrameMarkEnd(tracy_RenderSSAOTexture);

				FrameMarkStart(tracy_BlurSSAOTexture);
#endif

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoBlurredMap, 0);
				glClearColor(1.f, 1.f, 1.f, 1.f);
				glViewport(0, 0, wSize.x, wSize.y);
				glClear(GL_COLOR_BUFFER_BIT);

				_ssaoBlurredShader->Use();
				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_2D, _ssaoMap);
				_ssaoBlurredShader->SetInt("ssaoTexture", 0);

				glBindVertexArray(_screenPlaneVAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &_screenIndicies);
				glBindVertexArray(0);
				
#if TRACY_PROFILER
				FrameMarkEnd(tracy_BlurSSAOTexture);
#endif

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		if (_mode != CameraDisplayMode::DEPTH && _mode != CameraDisplayMode::SSAO_MAP) {
			// RENDER MAP
			glBindFramebuffer(GL_FRAMEBUFFER, _msRenderMapFBO);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, wSize.x, wSize.y);
			glEnable(GL_DEPTH_TEST);

			if (_mode == CameraDisplayMode::WIREFRAME) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

#if TRACY_PROFILER
			FrameMarkStart(tracy_RenderScreenTexture);
#endif

			BindSSAOTexture(31);
			BindDepthTexture(26);

			GraphicEngine::Render();

			ParticleSystemsController::Instance()->Update();
			ParticleSystemsController::Instance()->Render();

#if TRACY_PROFILER
			FrameMarkEnd(tracy_RenderScreenTexture);
#endif

			glBindFramebuffer(GL_READ_FRAMEBUFFER, _msRenderMapFBO);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderMapFBO);
			glBlitFramebuffer(0, 0, wSize.x, wSize.y, 0, 0, wSize.x, wSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	// RENDERING
	if (this->IsMain()) {

#if TRACY_PROFILER
		FrameMarkStart(tracy_OnScreenFramebuffer);
#endif

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, wSize.x, wSize.y);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		BindRenderTexture(0);
		BindDepthTexture(1);
		BindSSAOTexture(2);
		_screenShader->Use();
		_screenShader->SetInt("screenTexture", 0);
		_screenShader->SetInt("depthTexture", 1);
		_screenShader->SetInt("ssaoTexture", 2);

		_screenShader->SetInt("blurMSize", _blurMSize);
		_screenShader->SetFloatArray("blurKernel", _blurKernel, 40);

		_screenShader->SetBool("hasBlur", ((uint8_t)_filters & (uint8_t)CameraRenderFilter::BLUR) != 0);
		_screenShader->SetBool("hasVignette", (_filters & (uint8_t)CameraRenderFilter::VIGNETTE) != 0);
		_screenShader->SetBool("hasNegative", (_filters & (uint8_t)CameraRenderFilter::NEGATIVE) != 0);
		_screenShader->SetBool("hasGrayscale", (_filters & (uint8_t)CameraRenderFilter::GRAYSCALE) != 0);
		_screenShader->SetBool("hasOutline", (_filters & (uint8_t)CameraRenderFilter::OUTLINE) != 0);
		_screenShader->SetBool("hasDepthOfField", (_filters& (uint8_t)CameraRenderFilter::DEPTH_OF_FIELD) != 0);

		_screenShader->SetBool("depthOfField2", _depthOfField2);

		_screenShader->SetBool("displayDepth", _mode == CameraDisplayMode::DEPTH);
		_screenShader->SetBool("displaySSAO", _mode == CameraDisplayMode::SSAO_MAP);

		_screenShader->SetFloat("quadraticDepthOfField", _quadraticDepthOfField);
		_screenShader->SetFloat("linearDepthOfField", _linearDepthOfField);
		_screenShader->SetFloat("constantDepthOfField", _constantDepthOfField);

		_screenShader->SetFloat("brightness", _brightness);
		_screenShader->SetFloat("contrast", _contrast);

		glBindVertexArray(_screenPlaneVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &_screenIndicies);
		glBindVertexArray(0);

		ParticleSystemsController::Instance()->RenderUIBack();
		GraphicEngine::RenderGUI();
		ParticleSystemsController::Instance()->RenderUIFront();
		glBindTexture(GL_TEXTURE_2D, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
#if TRACY_PROFILER
		FrameMarkEnd(tracy_OnScreenFramebuffer);
#endif
	}
}

void CameraComponent::BindRenderTexture(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, _renderMap);
}

void CameraComponent::BindSSAOTexture(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurredMap);
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

Frustum CameraComponent::GetCurrentCameraFrustum()
{
	return _currentCameraFrustum;
}

void CameraComponent::Initialize()
{
	if (Cameras.size() == 0) {

		this->SetIsMain(true);

		// UBO MATRICIES
		glGenBuffers(1, &_uboCameraData);

		glBindBuffer(GL_UNIFORM_BUFFER, _uboCameraData);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboCameraData);

		_uboCameraDataStruct.Set("projection", this->GetProjectionMatrix());
		_uboCameraDataStruct.Set("view", this->GetViewMatrix());
		_uboCameraDataStruct.Set("viewPos", this->GetTransform()->GetGlobalPosition());
		_uboCameraDataStruct.Set("isSSAO", _isSsao);

		glBufferData(GL_UNIFORM_BUFFER, _uboCameraDataStruct.GetSize(), _uboCameraDataStruct.GetData().data(), GL_STATIC_DRAW);

		// UBO WINDOW DATA
		glGenBuffers(1, &_uboWindowData);

		glBindBuffer(GL_UNIFORM_BUFFER, _uboWindowData);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uboWindowData);

		_uboWindowDataStruct.Set("windowSize", Window::GetInstance()->GetContentSize());
		_uboWindowDataStruct.Set("nearPlane", this->_near);
		_uboWindowDataStruct.Set("farPlane", this->_far);
		_uboWindowDataStruct.Set("gamma", this->_gamma);
		_uboWindowDataStruct.Set("time", Time::GetTime());
		_uboWindowDataStruct.Set("deltaTime", Time::GetDeltaTime());

		glBufferData(GL_UNIFORM_BUFFER, _uboWindowDataStruct.GetSize(), _uboWindowDataStruct.GetData().data(), GL_STATIC_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glGenVertexArrays(1, &_screenPlaneVAO);
		glGenBuffers(1, &_screenPlaneVBO);
		glGenBuffers(1, &_screenPlaneEBO);

		glBindVertexArray(_screenPlaneVAO);
		glBindBuffer(GL_ARRAY_BUFFER, _screenPlaneVBO);

		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), _screenVerts, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _screenPlaneEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), _screenIndicies, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		_screenShader = ShaderManager::GetShaderProgram("origin/ScreenShader");
		_depthShader = ShaderManager::GetShaderProgram("origin/CameraDepthShader");
		_ssaoShader = ShaderManager::GetShaderProgram("origin/SSAOShader");
		_ssaoBlurredShader = ShaderManager::GetShaderProgram("origin/SSAOBlurredShader");

		GenerateSSAOKernel(48);
		GenerateSSAONoiseTexture();

		_ssaoShader->Use();
		for (size_t i = 0; i < 48; ++i) {
			_ssaoShader->SetVec3(string("kernel[").append(std::to_string(i)).append("]"), _ssaoKernel[i]);
		}

		TransformChangeEventId = GetTransform()->OnEventTransformChanged += [](Transform* transform) {
			LightingController::Instance()->UpdateOnTransformChange();
			glm::vec3 pos = transform->GetGlobalPosition();
			LightingController::Instance()->SetViewerPosition(pos);
		};
	}

	this->_camId = Cameras.size();
	Cameras.push_back(this);

	UpdateFrontDir();

	_transformEventId = GetTransform()->OnEventTransformChanged += [&](Transform* t) -> void { OnTransformChange(t); };
	_windowEventId = Window::GetInstance()->OnWindowSizeEvent += [&]() -> void { OnWindowSizeChange(); };

	ivec2 wSize = Window::GetInstance()->GetContentSize();

	unsigned int r_res = GL_RGB;
	unsigned int d_res = GL_DEPTH_COMPONENT;
	unsigned int s_res = GL_RED;

	switch (_renderRes) {
		case CameraRenderResolution::DEFAULT: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			s_res = GL_RED;
			break;
		}
		case CameraRenderResolution::MEDIUM: {
			r_res = GL_RGB16F;
			d_res = GL_DEPTH_COMPONENT16;
			s_res = GL_RED;
			break;
		}
		case CameraRenderResolution::HIGH: {
			r_res = GL_RGB32F;
			d_res = GL_DEPTH_COMPONENT32F;
			s_res = GL_R32F;
			break;
		}
		default: {
			r_res = GL_RGB;
			d_res = GL_DEPTH_COMPONENT;
			s_res = GL_RED;
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

#pragma region SSAOBuffer

	glGenFramebuffers(1, &_ssaoFBO);

	glGenTextures(1, &_ssaoMap);
	glBindTexture(GL_TEXTURE_2D, _ssaoMap);

	glTexImage2D(GL_TEXTURE_2D, 0, s_res, wSize.x / 2, wSize.y / 2, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &_ssaoBlurredMap);
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurredMap);

	glTexImage2D(GL_TEXTURE_2D, 0, s_res, wSize.x, wSize.y, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

	// SSAO Map
	glDeleteTextures(1, &_ssaoMap);
	glDeleteTextures(1, &_ssaoBlurredMap);
	glDeleteFramebuffers(1, &_ssaoFBO);

	// Render Map
	glDeleteTextures(1, &_renderMap);
	glDeleteTextures(1, &_msRenderMap);
	glDeleteFramebuffers(1, &_renderMapFBO);
	glDeleteFramebuffers(1, &_msRenderMapFBO);
	glDeleteRenderbuffers(1, &_msRenderBuffer);

	GetTransform()->OnEventTransformChanged -= _transformEventId;
	Window::GetInstance()->OnWindowSizeEvent -= _windowEventId;

	_transformEventId = 0;
	_windowEventId = 0;

	// TODO: CAN MAKE ERROR IF CAMERA IS IN MIDDLE
	Cameras.erase(Cameras.begin() + this->_camId);

	if (Cameras.size() == 0) {
		GetTransform()->OnEventTransformChanged -= TransformChangeEventId;
		TransformChangeEventId = 0;
		glDeleteTextures(1, &_ssaoNoiseTexture);
		delete _ssaoTextureData;
		glDeleteBuffers(1, &_uboCameraData);
		glDeleteBuffers(1, &_uboWindowData);
		glDeleteBuffers(1, &_screenPlaneEBO);
		glDeleteBuffers(1, &_screenPlaneVBO);
		glDeleteVertexArrays(1, &_screenPlaneVAO);
	}
}

Ray CameraComponent::GetScreenPointRay(glm::vec2 screenPosition) const
{
	ivec2 size = Window::GetInstance()->GetContentSize();
	glm::vec3 Origin = GetTransform()->GetGlobalPosition();

	//float x = 2.0f * screenPosition.x / size.x;	//2.0f * //Zakomentowane bo na razie jest niepotrzebne
	float y = 2.0f * screenPosition.y / size.y;	//2.0f * 
	//float x2 = 2.0f * screenPosition.x / size.x;	//
	//float y2 = 2.0f * screenPosition.y / size.y;	//

	//glm::vec3 Direction = _front + _right * (x * tanf(radians(_fov * 0.5)) * size.x / size.y) + cross(_right, _front) * (y * tanf(radians(_fov * 0.5))); // Zapis w razie czego
	float tanValue = tanf(radians(_fov * 0.5));
	glm::vec3 Direction = _front + _right * (2.0f * screenPosition.x * tanValue / size.y) + cross(_right, _front) * (y * tanValue);

	//glm::vec3 Direction2 = _front + _right * (x2 * tanf(radians(_fov * 0.5)) * size.x / size.y) + cross(_right, _front) * (y2 * tanf(radians(_fov * 0.5)));
	Direction = glm::normalize(Direction); 
	//SPDLOG_INFO("CP: \t{}\t{}\n\t\tF\t{}\t{}\t{}\n\t\tD\t{}\t{}\t{}\n\t\tD2\t{}\t{}\t{}", screenPosition.x, screenPosition.y, _front.x, _front.y, _front.z, Direction.x, Direction.y, Direction.z, Direction2.x, Direction2.y, Direction2.z);
	//float z = 1.0f; // NDC Z value (should be 1 for far plane)

	return Ray(std::move(Direction), std::move(Origin));
}

YAML::Node CameraComponent::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Camera";
	node["fov"] = _fov;
	node["nearPlane"] = _near;
	node["farPlane"] = _far;
	node["cameraFilter"] = (size_t)_filters;
	node["cameraType"] = _type;
	node["cameraMode"] = _mode;
	node["samples"] = (size_t)_samples;
	node["renderRes"] = _renderRes;
	node["gamma"] = _gamma;
	node["brightness"] = _brightness;
	node["contrast"] = _contrast;
	node["worldUp"] = _worldUp;
	node["isMain"] = _isMain;
	node["isFrustum"] = _isFrustumCulling;
	node["isSSAO"] = _isSsao;
	node["ssaoSampleRadius"] = _ssaoSampleRadius;
	node["ssaoBias"] = _ssaoBias;
	node["blurMSize"] = _blurMSize;
	return node;
}

bool CameraComponent::Deserialize(const YAML::Node& node) {
	if (!node["fov"] || !node["nearPlane"] || !node["farPlane"] ||
		!node["cameraFilter"] || !node["cameraType"] || !node["cameraMode"] ||
		!node["samples"] || !node["renderRes"] || !node["gamma"] || 
		!node["brightness"] || !node["contrast"] || !node["worldUp"] ||
		!node["isMain"] || !node["isFrustum"] || !node["isSSAO"] || 
		!node["ssaoSampleRadius"] || !node["ssaoBias"] || !node["blurMSize"] ||
		!Component::Deserialize(node)) return false;

	_fov = node["fov"].as<float>();
	_near = node["nearPlane"].as<float>();
	_far = node["farPlane"].as<float>();
	_filters = (uint8_t)node["cameraFilter"].as<size_t>();
	_type = node["cameraType"].as<CameraType>();
	_mode = node["cameraMode"].as<CameraDisplayMode>();
	_samples = (uint8_t)node["samples"].as<size_t>();
	_renderRes = node["renderRes"].as<CameraRenderResolution>();
	_gamma = node["gamma"].as<float>();
	_brightness = node["brightness"].as<float>();
	_contrast = node["contrast"].as<float>();
	_worldUp = node["worldUp"].as<vec3>();
	_isMain = node["isMain"].as<bool>();
	_isFrustumCulling = node["isFrustum"].as<bool>();
	_isSsao = node["isSSAO"].as<bool>();
	_ssaoSampleRadius = node["ssaoSampleRadius"].as<float>();
	_ssaoBias = node["ssaoBias"].as<float>();
	SetBlurMatrixSize(node["blurMSize"].as<size_t>());

	return true;
}

#if _DEBUG
void CameraComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Camera##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;
		bool per = (this->_type == CameraType::PERSPECTIVE);
		if (ImGui::BeginCombo(string("Projection##").append(id).c_str(), (per ? "Perspective" : "Orthographic"))) {
			if (ImGui::Selectable(string("Orthographic##").append(id).c_str(), !per))
			{
				this->SetCameraType(CameraType::ORTHOGRAPHIC);
			}
			else if (ImGui::Selectable(string("Perspective##").append(id).c_str(), per))
			{
				this->SetCameraType(CameraType::PERSPECTIVE);
			}
			ImGui::EndCombo();
		}

		CameraRenderResolution res = this->_renderRes;
		if (ImGui::BeginCombo(string("Render Resolution##").append(id).c_str(), res == CameraRenderResolution::DEFAULT ? "Default" : (res == CameraRenderResolution::MEDIUM ? "Medium" : "High")))
		{
			if (ImGui::Selectable(string("Default##").append(id).c_str(), res == CameraRenderResolution::DEFAULT))
			{
				this->SetRenderResolution(CameraRenderResolution::DEFAULT);
			}
			else if (ImGui::Selectable(string("Medium##").append(id).c_str(), res == CameraRenderResolution::MEDIUM))
			{
				this->SetRenderResolution(CameraRenderResolution::MEDIUM);
			}
			else if (ImGui::Selectable(string("High##").append(id).c_str(), res == CameraRenderResolution::HIGH))
			{
				this->SetRenderResolution(CameraRenderResolution::HIGH);
			}
			ImGui::EndCombo();
		}

		CameraDisplayMode cdm = this->_mode;
		if (ImGui::BeginCombo(string("Display Mode##").append(id).c_str(), cdm == CameraDisplayMode::RENDER ? "Render" : (cdm == CameraDisplayMode::DEPTH ? "Depth" : (cdm == CameraDisplayMode::SSAO_MAP ? "SSAO" : "Wireframe"))))
		{
			if (ImGui::Selectable(string("Render##").append(id).c_str(), cdm == CameraDisplayMode::RENDER))
			{
				this->SetDisplayMode(CameraDisplayMode::RENDER);
			}
			else if (ImGui::Selectable(string("Depth##").append(id).c_str(), cdm == CameraDisplayMode::DEPTH))
			{
				this->SetDisplayMode(CameraDisplayMode::DEPTH);
			}
			else if (ImGui::Selectable(string("SSAO##").append(id).c_str(), cdm == CameraDisplayMode::SSAO_MAP))
			{
				this->SetDisplayMode(CameraDisplayMode::SSAO_MAP);
			}
			else if (ImGui::Selectable(string("Wireframe##").append(id).c_str(), cdm == CameraDisplayMode::WIREFRAME))
			{
				this->SetDisplayMode(CameraDisplayMode::WIREFRAME);
			}
			ImGui::EndCombo();
		}

		uint8_t acFil = (uint8_t)CameraRenderFilter::NONE;
		uint8_t fil = (uint8_t)this->_filters;

		bool n = (fil ^ (uint8_t)CameraRenderFilter::NONE) == 0;
		ImGui::Checkbox(string("Nothing##").append(id).c_str(), &n);

		bool e = (fil ^ (uint8_t)CameraRenderFilter::EVERYTHING) == 0 && !n;
		bool g = (fil ^ (uint8_t)CameraRenderFilter::EVERYTHING) == 0 && !n;
		ImGui::Checkbox(string("Everything##").append(id).c_str(), &g);
		if (g) {
			acFil |= (uint8_t)CameraRenderFilter::EVERYTHING;
		}

		g = (fil & (uint8_t)CameraRenderFilter::GRAYSCALE) != 0 && !n;
		ImGui::Checkbox(string("GrayScale##").append(id).c_str(), &g);
		if (g) {
			acFil |= (uint8_t)CameraRenderFilter::GRAYSCALE;
		}
		else {
			if (e) {
				acFil ^= (uint8_t)CameraRenderFilter::GRAYSCALE;
			}
		}

		g = (fil & (uint8_t)CameraRenderFilter::NEGATIVE) != 0 && !n;
		ImGui::Checkbox(string("Negative##").append(id).c_str(), &g);
		if (g) {
			acFil |= (uint8_t)CameraRenderFilter::NEGATIVE;
		}
		else {
			if (e) {
				acFil ^= (uint8_t)CameraRenderFilter::NEGATIVE;
			}
		}

		g = (fil & (uint8_t)CameraRenderFilter::VIGNETTE) != 0 && !n;
		ImGui::Checkbox(string("Vignette##").append(id).c_str(), &g);
		if (g) {
			acFil |= (uint8_t)CameraRenderFilter::VIGNETTE;
		}
		else {
			if (e) {
				acFil ^= (uint8_t)CameraRenderFilter::VIGNETTE;
			}
		}

		g = (fil & (uint8_t)CameraRenderFilter::BLUR) != 0 && !n;
		ImGui::Checkbox(string("Blur##").append(id).c_str(), &g);
		if (g) {
			acFil |= (uint8_t)CameraRenderFilter::BLUR;
		}
		else {
			if (e) {
				acFil ^= (uint8_t)CameraRenderFilter::BLUR;
			}
		}

		g = (fil & (uint8_t)CameraRenderFilter::OUTLINE) != 0 && !n;
		ImGui::Checkbox(string("Outline##").append(id).c_str(), &g);
		if (g) {
			acFil |= (uint8_t)CameraRenderFilter::OUTLINE;
		}
		else {
			if (e) {
				acFil ^= (uint8_t)CameraRenderFilter::OUTLINE;
			}
		}

		g = (fil & (uint8_t)CameraRenderFilter::DEPTH_OF_FIELD) != 0 && !n;
		ImGui::Checkbox(string("Depth of Field##").append(id).c_str(), &g);
		if (g) {
			acFil |= (uint8_t)CameraRenderFilter::DEPTH_OF_FIELD;
		}
		else {
			if (e) {
				acFil ^= (uint8_t)CameraRenderFilter::DEPTH_OF_FIELD;
			}
		}

		this->SetCameraFilter(acFil);

		ImGui::BeginDisabled((acFil& (uint8_t)CameraRenderFilter::DEPTH_OF_FIELD) == 0);
		ImGui::Checkbox(string("Depth Of Field 2##").append(id).c_str(), &this->_depthOfField2);
		ImGui::EndDisabled();

		if (this->_depthOfField2) {
			
			float v = this->_quadraticDepthOfField;
			if (ImGui::InputFloat(string("Quadratic Depth Of Field##").append(id).c_str(), &v)) {
				if (v != this->_quadraticDepthOfField) {
					this->_quadraticDepthOfField = v;
				}
			}
			
			v = this->_linearDepthOfField;
			if (ImGui::InputFloat(string("Linear Depth Of Field##").append(id).c_str(), &v)) {
				if (v != this->_linearDepthOfField) {
					this->_linearDepthOfField = v;
				}
			}
			
			v = this->_constantDepthOfField;
			if (ImGui::InputFloat(string("Constant Depth Of Field##").append(id).c_str(), &v)) {
				if (v != this->_constantDepthOfField) {
					this->_constantDepthOfField = v;
				}
			}

			unsigned int mSize = this->_blurMSize;
			if (ImGui::DragUInt(string("Gaussian Blur Power##").append(id).c_str(), &mSize, 1.0f, 0, 40)) {
				SetBlurMatrixSize(mSize);
			}
		}

		int s = (int)this->_samples;
		ImGui::InputInt(string("MSAA Samples##").append(id).c_str(), &s);

		if (s != (int)this->_samples) {
			this->SetSamples((uint8_t)s);
		}

		ImGui::InputFloat(string("Near Plane##").append(id).c_str(), &this->_near);
		ImGui::InputFloat(string("Far Plane##").append(id).c_str(), &this->_far);
		ImGui::InputFloat(string("Gamma##").append(id).c_str(), &this->_gamma);
		ImGui::DragFloat(string("Brightness##").append(id).c_str(), &this->_brightness, 0.01f, -1.f, 1.f);
		ImGui::DragFloat(string("Contrast##").append(id).c_str(), &this->_contrast, 0.01f, 0.0f, FLT_MAX);
		ImGui::Checkbox(string("SSAO##").append(id).c_str(), &this->_isSsao);
		if (this->_isSsao) {
			ImGui::DragFloat(string("SSAO Bias##").append(id).c_str(), &this->_ssaoBias, 0.1f);
			ImGui::DragFloat(string("SSAO Sample Radius##").append(id).c_str(), &this->_ssaoSampleRadius, 0.1f);
		}
		ImGui::Checkbox(string("Frustum Culling##").append(id).c_str(), &this->_isFrustumCulling);
	}
}
#endif