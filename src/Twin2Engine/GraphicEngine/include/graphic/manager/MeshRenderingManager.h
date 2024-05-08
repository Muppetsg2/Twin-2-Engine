#pragma once

#include <graphic/InstantiatingMesh.h>
#include <graphic/Material.h>
#include <graphic/Shader.h>
#include <core/CameraComponent.h>
#include <core/GameObject.h>
#include <core/MeshRenderer.h>

// RENDERING
#define MAX_INSTANCE_NUMBER_PER_DRAW 1024
#define MAX_MATERIAL_NUMBER_PER_DRAW 8
#define MAX_MATERIAL_SIZE 1024

// SSBO
#define BINDING_POINT_INSTANCE_DATA 0
#define BINDING_POINT_MATERIAL_INDEX 1

// UBO
#define BINDING_POINT_MATERIAL_INPUT 2

#define RENERING_TYPE_MESH_SHADER_MATERIAL false
#define RENERING_TYPE_SHADER_MATERIAL_MESH true
#define RENERING_TYPE_SHADER_MESH_MATERIAL true

#define USE_NAMED_BUFFER_SUBDATA 0

#define MATERIAL_INPUT_SINGLE_UBO false
#define MATERIAL_INPUT_MANY_INPUT true

namespace Twin2Engine
{
	namespace GraphicEngine {
		class GraphicEngineManager;
		class InstatiatingMesh;
		class Material;
	}

	namespace Core {
		class MeshRenderer;
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
			struct RenderedSegment
			{
				unsigned int offset;
				unsigned int count;
			};
			struct MeshRenderingData
			{
				std::vector<glm::mat4> modelTransforms;
				std::vector<Twin2Engine::Core::MeshRenderer*> meshRenderers;
				std::list<RenderedSegment> rendered;
				unsigned int renderedCount;
			};
			struct RenderedSegmentDepthMap
			{
				std::vector<glm::mat4>* modelTransforms;
				unsigned int offset;
				unsigned int count;
			};
			struct MeshRenderingDataDepthMap
			{
				std::list<RenderedSegmentDepthMap> rendered;
				unsigned int renderedCount;
			};

#if RENERING_TYPE_MESH_SHADER_MATERIAL
			static std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;
#elif RENERING_TYPE_SHADER_MATERIAL_MESH
			static std::unordered_map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::unordered_map<GraphicEngine::InstatiatingMesh*, MeshRenderingData>>>  _renderQueueStatic;
			static std::unordered_map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::unordered_map<GraphicEngine::InstatiatingMesh*, MeshRenderingData>>>  _depthMapenderQueueStatic;

			static std::unordered_map<GraphicEngine::InstatiatingMesh*, MeshRenderingDataDepthMap> _depthQueueStatic;


			static std::unordered_map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::unordered_map<GraphicEngine::InstatiatingMesh*, MeshRenderingData>>>  _renderQueueDynamic;
			static std::unordered_map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::unordered_map<GraphicEngine::InstatiatingMesh*, MeshRenderingData>>>  _depthMapenderQueueDynamic;

			static std::unordered_map<GraphicEngine::InstatiatingMesh*, MeshRenderingDataDepthMap> _depthQueueDynamic;


			static std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::map<GraphicEngine::InstatiatingMesh*, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::map<GraphicEngine::InstatiatingMesh*, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;
#elif RENERING_TYPE_SHADER_MESH_MATERIAL
			static std::map<GraphicEngine::Shader*, std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<GraphicEngine::Shader*, std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;
#endif
			static std::map<GraphicEngine::InstatiatingMesh*, std::queue<MeshRenderData>> _depthQueue;

			static GLuint _instanceDataSSBO;
			static GLuint _materialIndexSSBO;
			static GLuint _materialInputUBO;

			static glm::mat4* _modelTransforms;
			static unsigned int* _materialsIndexes;

			static void UpdateQueues();

			static void RenderStatic();
			static void RenderDepthMapStatic();

			static void Render();
			static void RenderDepthMap();
		public:
			static void Init();
			static void UnloadAll();

			static void RegisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer);
			static void UnregisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer);

			static void RegisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer);
			static void UnregisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer);

			static void Render(MeshRenderData meshData);
			static void RenderDepthMap(const unsigned int& bufferWidth, const unsigned int& bufferHeight, const GLuint& depthFBO, const GLuint& depthMapTex,
				glm::mat4& projectionViewMatrix);
			static void RenderDepthMapStatic(const unsigned int& bufferWidth, const unsigned int& bufferHeight, const GLuint& depthFBO, const GLuint& depthMapTex,
				glm::mat4& projectionViewMatrix);
		};
	}
}