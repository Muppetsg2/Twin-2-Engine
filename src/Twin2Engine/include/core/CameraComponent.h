#pragma once

#include <core/Component.h>
#include <core/Frustum.h>

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
		GRAYSCALE = 8
	};

	class CameraComponent : public Component {
	private:
		static GLuint _uboMatrices;
		static GLuint _uboWindowData;
		static Twin2Engine::GraphicEngine::InstatiatingModel _renderPlane;
		static Twin2Engine::GraphicEngine::Shader* _renderShader;

		GLuint _depthMapFBO;
		GLuint _depthMap;

		GLuint _renderBuffer;
		GLuint _renderMapFBO;
		GLuint _renderMap;

		CameraType _type = PERSPECTIVE;

		uint8_t _filters = NONE;

		size_t _camId = 0;

		bool _isMain = false;

		float _near = 0.1f;
		float _far = 1000.f;
		float _fov = 45.f;

		vec3 _front = vec3(0.f, 0.f, -1.f);
		vec3 _right = vec3(1.f, 0.f, 0.f);
		vec3 _up = vec3(0.f, 1.f, 0.f);
		vec3 _worldUp = vec3(0.f, 1.f, 0.f);

		size_t _transformEventId;
		void OnTransformChange(Transform* trans);
		size_t _windowEventId;
		void OnWindowSizeChange();

	public:
		static std::vector<CameraComponent*> Cameras;

		CameraType GetCameraType();
		uint8_t GetCameraFilters();

		float GetNearPlane() const;
		float GetFarPlane() const;
		float GetFOV() const;
		vec3 GetFrontDir() const;
		vec3 GetWorldUp() const;
		vec3 GetRight() const;
		mat4 GetViewMatrix() const;
		mat4 GetProjectionMatrix();
		Frustum GetFrustum();

		bool IsMain() const;

		void SetFOV(float angle);
		void SetFarPlane(float value);
		void SetNearPlane(float value);
		
		void SetCameraFilter(uint8_t filters);
		void SetCameraType(CameraType value);

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

		/*
		void Update() override;
		void OnEnable() override;
		void OnDisable() override;
		*/
	};
}