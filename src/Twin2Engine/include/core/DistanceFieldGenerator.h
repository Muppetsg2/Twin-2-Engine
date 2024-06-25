#pragma once

#pragma once

#include <glm/glm.hpp>
#include <core/Component.h>
#include <tools/STD140Struct.h>

namespace Twin2Engine::Graphic {
	class InstantiatingMesh;
	class Shader;
}

namespace Twin2Engine::Core {
	class DistanceFieldGenerator : public Component {

		ENUM_CLASS_BASE(LookingDir, uint8_t, FRONT, BACK, UP, DOWN, RIGHT, LEFT);

		// UBO DATA
		static GLuint _uboDistanceFieldData;
		static Tools::STD140Struct _uboDistanceFieldDataStruct;

		// PLANE
		static bool _planeCreated;
		static GLuint _planeVAO;
		static GLuint _planeVBO;
		static GLuint _planeEBO;
		static float _planeVerts[];
		static GLuint _planeIndicies[];

		// GAUSSIAN BLUR
		static size_t _blurMSize;
		static float _blurKernel[40];

		// SHADERS
		static Graphic::Shader* _depthShader;
		static Graphic::Shader* _blurShader;

		// DIRECTION
		LookingDir _dir = LookingDir::FRONT;
		glm::vec3 _front = glm::vec3(0.f, 0.f, -1.f);
		glm::vec3 _right = glm::vec3(1.f, 0.f, 0.f);
		glm::vec3 _up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 _worldUp = glm::vec3(0.f, 1.f, 0.f);

		// VIEW
		//float _near = 0.1f;
		//float _far = 1000.f;
		//float _fov = 45.f;
		float _orthoSize = 5.f;

		// RENDER FBO
		GLuint _depthFBO = NULL;
		GLuint _blurFBO = NULL;

		// RENDER MAPS
		GLuint _depthMap = NULL;
		GLuint _texture = NULL;

		bool _generated = false;
		unsigned int _width = 1024;
		unsigned int _height = 1024;

		void CreatePlane();
		void GenerateBlurKernel();
		void GenerateTexture();
		void UpdateUBOData();

	public:
		virtual void Initialize() override;
		virtual void Update() override;
		//virtual void OnDestroy() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		virtual void DrawEditor() override;
#endif
		void UseTexture(unsigned int i = 0);

		LookingDir GetLookingDir();

		unsigned int GetTextureId();
		unsigned int GetTextureWidth();
		unsigned int GetTextureHeight();
		glm::uvec2 GetTextureDimmensions();

		void SetLookingDir(LookingDir dir);

		void SetTextureWidth(unsigned int width);
		void SetTextureHeight(unsigned int height);
		void SetTextureDimmensions(glm::uvec2 dim);
	};
}