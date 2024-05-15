#pragma once

#include <core/Component.h>
#include <graphic/Frustum.h>
#include <physic/Ray.h>
#include <tools/STD140Struct.h>

using namespace glm;

namespace Twin2Engine::Graphic {
	class InstantiatingModel;
	class Shader;
}

namespace Twin2Engine::Core {
	enum class CameraType{
		ORTHOGRAPHIC = 0,
		PERSPECTIVE = 1
	};

	enum class CameraRenderFilter : uint8_t {
		NONE = 0,
		VIGNETTE = 1,
		BLUR = 2,
		NEGATIVE = 4,
		GRAYSCALE = 8,
		OUTLINE = 16,
		EVERYTHING = VIGNETTE | BLUR | NEGATIVE | GRAYSCALE | OUTLINE
	};

	enum class CameraDisplayMode {
		RENDER = 0,
		DEPTH = 1,
		SSAO_MAP = 2
	};

	enum class CameraRenderResolution {
		DEFAULT = 0,
		MEDIUM = 1,
		HIGH = 2
	};

	class CameraComponent : public Component {
	private:
		static GLuint _uboCameraData;
		static Tools::STD140Offsets _uboCameraDataOffsets;
		static GLuint _uboWindowData;
		static Tools::STD140Offsets _uboWindowDataOffsets;
		static Graphic::InstantiatingModel _screenPlane;
		static Graphic::Shader* _screenShader;
		static Graphic::Shader* _ssaoShader;
		static Graphic::Shader* _ssaoBlurredShader;
		static Graphic::Shader* _depthShader;
		static Graphic::Frustum _currentCameraFrustum;
		static std::vector<glm::vec3> _ssaoKernel;
		static float* _ssaoTextureData;
		static GLuint _ssaoNoiseTexture;

		// Depth Pre pass
		GLuint _depthMapFBO = NULL;
		GLuint _depthMap = NULL;

		// SSAO
		GLuint _ssaoFBO = NULL;
		GLuint _ssaoMap = NULL;
		GLuint _ssaoBlurredMap = NULL;

		// MSAA Render
		GLuint _msRenderMapFBO = NULL;
		GLuint _msRenderMap = NULL;
		GLuint _msRenderBuffer = NULL;

		GLuint _renderMap = NULL;
		GLuint _renderMapFBO = NULL;

		CameraType _type = CameraType::PERSPECTIVE;
		CameraDisplayMode _mode = CameraDisplayMode::RENDER;
		uint8_t _filters = (uint8_t)CameraRenderFilter::NONE;
		uint8_t _samples = 4;
		CameraRenderResolution _renderRes = CameraRenderResolution::DEFAULT;

		size_t _camId = 0;

		bool _isMain = false;
		bool _isInit = false;
		bool _isFrustumCulling = true;
		bool _isSsao = true;

		float _near = 0.1f;
		float _far = 1000.f;
		float _fov = 45.f;
		float _gamma = 2.2f;

		vec3 _front = vec3(0.f, 0.f, -1.f);
		vec3 _right = vec3(1.f, 0.f, 0.f);
		vec3 _up = vec3(0.f, 1.f, 0.f);
		vec3 _worldUp = vec3(0.f, 1.f, 0.f);

		size_t _transformEventId = 0;
		void OnTransformChange(Transform* trans);
		size_t _windowEventId = 0;
		void OnWindowSizeChange();
		void SetFrontDir(vec3 dir);
		void GenerateSSAOKernel();
		void GenerateSSAONoiseTexture();

	public:
		static std::vector<CameraComponent*> Cameras;

		CameraType GetCameraType() const;
		uint8_t GetCameraFilters() const;
		uint8_t GetSamples() const;
		CameraRenderResolution GetRenderResolution() const;
		CameraDisplayMode GetDisplayMode() const;

		float GetFOV() const;
		float GetGamma() const;
		float GetNearPlane() const;
		float GetFarPlane() const;
		vec3 GetFrontDir() const;
		vec3 GetWorldUp() const;
		vec3 GetRight() const;
		mat4 GetViewMatrix() const;
		mat4 GetProjectionMatrix() const;
		Graphic::Frustum GetFrustum() const;

		bool IsMain() const;
		bool IsFrustumCullingOn() const;
		bool IsSSAO() const;

		void SetFOV(float angle);
		void SetGamma(float gamma);
		void SetFarPlane(float value);
		void SetNearPlane(float value);
		
		void SetCameraFilter(uint8_t filters);
		void SetCameraType(CameraType value);
		void SetSamples(uint8_t i = 4);
		void SetRenderResolution(CameraRenderResolution res);
		void SetDisplayMode(CameraDisplayMode cdm);

		void SetWorldUp(vec3 value);

		void UpdateFrontDir();

		void SetIsMain(bool value);
		void SetFrustumCulling(bool value);
		void SetSSAO(bool value);

		void Render();

		void BindRenderTexture(unsigned int index = 0);
		void BindSSAOTexture(unsigned int index = 0);
		void BindDepthTexture(unsigned int index = 0);

		static CameraComponent* GetMainCamera();
		static Graphic::Frustum GetCurrentCameraFrustum();

		void Initialize() override;
		void OnDestroy() override;
		YAML::Node Serialize() const override;
		void DrawEditor() override;
		Physic::Ray GetScreenPointRay(glm::vec2 screenPosition) const;
	};
}

namespace YAML {
	template<> struct convert<Twin2Engine::Core::CameraType> {
		using CameraType = Twin2Engine::Core::CameraType;

		static Node encode(const CameraType& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, CameraType& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (CameraType)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Core::CameraRenderResolution> {
		using CameraRenderResolution = Twin2Engine::Core::CameraRenderResolution;

		static Node encode(const CameraRenderResolution& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, CameraRenderResolution& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (CameraRenderResolution)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Core::CameraDisplayMode> {
		using CameraDisplayMode = Twin2Engine::Core::CameraDisplayMode;

		static Node encode(const CameraDisplayMode& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, CameraDisplayMode& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (CameraDisplayMode)node.as<size_t>();
			return true;
		}
	};
}