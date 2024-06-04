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
	namespace Graphic {
		class GraphicEngine;
		class InstantiatingMesh;
		class Material;
	}

	namespace Core {
		class MeshRenderer;
	}

	namespace Manager {

		//struct InstanceData
		//{
		//	glm::mat4 transformMatrix;
		//	unsigned int materialInputId;
		//};
		//
		//struct MeshRenderData {
		//	std::vector<Graphic::InstantiatingMesh*> meshes;
		//	std::vector<Graphic::Material> materials;
		//	glm::mat4 transform;
		//	bool isTransparent;
		//};

		class MeshRenderingManager
		{
			friend class Twin2Engine::Graphic::GraphicEngine;

		private:
			struct RenderedSegment
			{
				glm::mat4* begin;
				unsigned int count;
			};
			struct MeshRenderingData
			{
				std::vector<glm::mat4> modelTransforms;
				std::vector<Twin2Engine::Core::MeshRenderer*> meshRenderers;
				std::list<RenderedSegment> rendered;
				unsigned int renderedCount;
			};
			struct MeshRenderingDataDepthMap
			{
				std::list<RenderedSegment> rendered;
				unsigned int renderedCount;
			};
			struct DataToUnregister {
				Graphic::Material _mat;
				Graphic::InstantiatingMesh* _mesh;
				size_t _pos;
			};

			struct Flags {
				bool IsStaticChanged : 1;
			};

			static Flags _flags;

			static std::unordered_map<Graphic::Shader*, std::map<Graphic::Material, std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingData>>>  _renderQueueStatic;
			static std::unordered_map<Graphic::Shader*, std::map<Graphic::Material, std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingData>>>  _renderQueueStaticTransparent;
			//static std::unordered_map<Graphic::Shader*, std::map<Graphic::Material, std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingData>>>  _depthMapenderQueueStatic;

			static std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingDataDepthMap> _depthMapQueueStatic;
			static std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingDataDepthMap> _depthQueueStatic;


			static std::unordered_map<Graphic::Shader*, std::map<Graphic::Material, std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingData>>>  _renderQueueDynamic;
			static std::unordered_map<Graphic::Shader*, std::map<Graphic::Material, std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingData>>>  _renderQueueDynamicTransparent;
			//static std::unordered_map<Graphic::Shader*, std::map<Graphic::Material, std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingData>>>  _depthMapenderQueueDynamic;

			static std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingDataDepthMap> _depthMapQueueDynamic;
			static std::unordered_map<Graphic::InstantiatingMesh*, MeshRenderingDataDepthMap> _depthQueueDynamic;


			static GLuint _instanceDataSSBO;
			static GLuint _materialIndexSSBO;
			static GLuint _materialInputUBO;

			static glm::mat4* _modelTransforms;
			static unsigned int* _materialsIndexes;

			static void UpdateQueues();

			static void PreRender();
			static void RenderStatic();

		public:
			static void Init();
			static void UnloadAll();

			static bool RegisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer);
			static bool UnregisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer);

			static bool RegisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer);
			static bool UnregisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer);

			//Przed u¿yciem tej funkcji nale¿y zapewniæ, i¿ glViewport jest ustawiony w nastêpuj¹cy sposób: glViewport(0, 0, depthTexWidth, depthTexHeight), po uruchomieñiu funkcji nale¿y przywróciæ rozmiar viewportu do rozmiaru okna gry
			static void RenderDepthMapDynamic(const GLuint& depthFBO, glm::mat4& projectionViewMatrix);
			static void RenderDepthMapStatic(const GLuint& depthFBO, const GLuint& depthREplacingTexId, const GLuint& depthReplcedTexId, glm::mat4& projectionViewMatrix);
			static void RenderCloudDepthMap(std::unordered_map<Twin2Engine::Graphic::InstantiatingMesh*, std::vector<Twin2Engine::Core::Transform*>>& depthQueue) {
				std::vector<glm::mat4> transformationMatrixes;
				for (auto& pair : depthQueue) {
					if (pair.second.size() != 0) {
						for (auto& t : pair.second) {
							transformationMatrixes.push_back(t->GetTransformMatrix());
						}

						glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
						glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * pair.second.size(), transformationMatrixes.data());
						glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
						pair.first->Draw(pair.second.size());

						GLenum error = glGetError();
						if (error != GL_NO_ERROR) {
							SPDLOG_ERROR("Error: {}", error);
						}
						transformationMatrixes.clear();
					}
				}
			}
		};
	}
}