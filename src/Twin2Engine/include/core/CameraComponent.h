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
		CameraType _type = PERSPECTIVE;

		bool _isMain = false;

		int32_t _w_width = 1920;
		int32_t _w_height = 1080;
		float _near = 0.1f;
		float _far = 1000.f;
		float _fov = 45.f;

		vec3 _front = vec3(0.f, 0.f, -1.f);
		vec3 _right = vec3(1.f, 0.f, 0.f);
		vec3 _up = vec3(0.f, 1.f, 0.f);
		vec3 _worldUp = vec3(0.f, 1.f, 0.f);
	public:			
		CameraType GetCameraType();
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

		void SetWindowSize(vec2 size);
		void SetFOV(float angle);
		void SetFarPlane(float value);
		void SetNearPlane(float value);
		void SetCameraType(CameraType value);
		void SetFrontDir(vec3 dir);
		void SetWorldUp(vec3 value);

		void SetIsMain(bool value);

		/*
		virtual void Initialize();
		virtual void Update();
		virtual void OnEnable();
		virtual void OnDisable();
		virtual void OnDestroy();
		*/
	};
}