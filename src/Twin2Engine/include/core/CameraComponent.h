#pragma once

#include <core/Component.h>
#include <core/Frustum.h>
#include <Ray.h>

using namespace glm;

namespace Twin2Engine::GraphicEngine {
	class InstatiatingModel;
	class Shader;
}

namespace Twin2Engine::Core {
	enum CameraType {
		ORTHOGRAPHIC = 0,
		PERSPECTIVE = 1
	};

	enum RenderFilter {
		NONE = 0,
		VIGNETTE = 1,
		BLUR = 2,
		NEGATIVE = 4,
		GRAYSCALE = 8,
		DEPTH = 16,
		OUTLINE = 32
	};

	enum RenderResolution {
		DEFAULT = 0,
		MEDIUM = 1,
		HIGH = 2
	};

	class CameraComponent : public Component {
	private:
		static GLuint _uboMatrices;
		static GLuint _uboWindowData;
		static Twin2Engine::GraphicEngine::InstatiatingModel _renderPlane;
		static Twin2Engine::GraphicEngine::Shader* _renderShader;

		GLuint _depthMapFBO = NULL;
		GLuint _depthMap = NULL;

		// MSAA Render
		GLuint _msRenderMapFBO = NULL;
		GLuint _msRenderMap = NULL;
		GLuint _msRenderBuffer = NULL;

		GLuint _renderMap = NULL;
		GLuint _renderMapFBO = NULL;

		CameraType _type = PERSPECTIVE;
		uint8_t _filters = NONE;
		uint8_t _samples = 4;
		RenderResolution _renderRes = DEFAULT;

		size_t _camId = 0;

		bool _isMain = false;
		bool _isInit = false;

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

	public:
		static std::vector<CameraComponent*> Cameras;
		bool IsFrustumCullingOn = true;

		CameraType GetCameraType() const;
		uint8_t GetCameraFilters() const;
		uint8_t GetSamples() const;
		RenderResolution GetRenderResolution() const;

		float GetFOV() const;
		float GetGamma() const;
		float GetNearPlane() const;
		float GetFarPlane() const;
		vec3 GetFrontDir() const;
		vec3 GetWorldUp() const;
		vec3 GetRight() const;
		mat4 GetViewMatrix() const;
		mat4 GetProjectionMatrix() const;
		Frustum GetFrustum() const;

		bool IsMain() const;

		void SetFOV(float angle);
		void SetGamma(float gamma);
		void SetFarPlane(float value);
		void SetNearPlane(float value);
		
		void SetCameraFilter(uint8_t filters);
		void SetCameraType(CameraType value);
		void SetSamples(uint8_t i = 4);
		void SetRenderResolution(RenderResolution res);

		void SetFrontDir(vec3 dir);
		void SetWorldUp(vec3 value);

		void SetIsMain(bool value);

		void Render();

		void BindRenderTexture(unsigned int index = 0);
		void BindDepthTexture(unsigned int index = 0);

		// Jesli beda sceny to tu trzeba dodac by scena byla przekazywana
		static CameraComponent* GetMainCamera();

		void Initialize() override;
		void OnDestroy() override;
		YAML::Node Serialize() const override;

		/*
		void Update() override;
		void OnEnable() override;
		void OnDisable() override;
		*/

		CollisionSystem::Ray GetScreenPointRay(glm::vec2 screenPosition);
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

	template<> struct convert<Twin2Engine::Core::RenderResolution> {
		using RenderResolution = Twin2Engine::Core::RenderResolution;

		static Node encode(const RenderResolution& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, RenderResolution& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (RenderResolution)node.as<size_t>();
			return true;
		}
	};
}