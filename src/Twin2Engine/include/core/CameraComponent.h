#pragma once

#include <core/Component.h>
#include <core/Frustum.h>

using namespace glm;

namespace Twin2Engine::Core {
	enum CameraType {
		ORTHOGRAPHIC = 0,
		PERSPECTIVE = 1
	};

	class CameraComponent : public Component {
	private:
		static GLuint _uboMatrices;

		GLuint _depthMapFBO = NULL;
		GLuint _depthMap = NULL;

		GLuint _renderMapFBO = NULL;
		GLuint _renderMap = NULL;

		CameraType _type = PERSPECTIVE;

		size_t _camId = 0;

		bool _isMain = false;

		float _near = 0.1f;
		float _far = 1000.f;
		float _fov = 45.f;

		vec3 _front = vec3(0.f, 0.f, -1.f);
		vec3 _right = vec3(1.f, 0.f, 0.f);
		vec3 _up = vec3(0.f, 1.f, 0.f);
		vec3 _worldUp = vec3(0.f, 1.f, 0.f);

		size_t _eventId = 0;
		void OnTransformChange(Transform* trans);

	public:
		static std::vector<CameraComponent*> Cameras;

		CameraType GetCameraType() const;
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
		void SetCameraType(CameraType value);
		void SetFrontDir(vec3 dir);
		void SetWorldUp(vec3 value);

		void SetIsMain(bool value);

		void StartDepthTest();
		void StartRender();

		void EndDepthTest();
		void EndRender();

		GLuint GetRenderTexture() const;
		GLuint GetDepthTexture() const;

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