#pragma once

#include <graphic/InstatiatingMesh.h>
#include <graphic/Material.h>
#include <graphic/Shader.h>

// RENDERING
#define MAX_INSTANCE_NUMBER_PER_DRAW 1024
#define MAX_MATERIAL_NUMBER_PER_DRAW 8
#define MAX_MATERIAL_SIZE 1024

// SSBO
#define BINDING_POINT_INSTANCE_DATA 0
#define BINDING_POINT_MATERIAL_INDEX 1

// UBO
#define BINDING_POINT_MATERIAL_INPUT 2

namespace Twin2Engine
{
	namespace GraphicEngine {
		class GraphicEngineManager;
		class InstatiatingMesh;
		class Material;
	}

	namespace Manager {

		struct InstanceData
		{
			glm::mat4 transformMatrix;
			unsigned int materialInputId;
		};

		struct MeshRenderData {
			std::vector<GraphicEngine::InstatiatingMesh*> meshes;
			std::vector<GraphicEngine::Material> materials;
			glm::mat4 transform;
			bool isTransparent;
		};

		class MeshRenderingManager
		{
			friend class GraphicEngine::GraphicEngineManager;

		private:
			static std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;
			static std::map<GraphicEngine::InstatiatingMesh*, std::queue<MeshRenderData>> _depthQueue;

			static GLuint _instanceDataSSBO;
			static GLuint _materialIndexSSBO;
			static GLuint _materialInputUBO;

			static void Render();
			static void RenderDepthMap();
		public:
			static void Init();
			static void UnloadAll();

			static void Render(MeshRenderData meshData);
			static void RenderDepthMap(const unsigned int& bufferWidth, const unsigned int& bufferHeight, const GLuint& depthFBO, const GLuint& depthMapTex,
									   glm::mat4& projectionViewMatrix);
		};
	}
}