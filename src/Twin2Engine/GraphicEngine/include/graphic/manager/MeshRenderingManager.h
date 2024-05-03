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
			std::vector<Graphic::InstatiatingMesh*> meshes;
			std::vector<Graphic::Material> materials;
			glm::mat4 transform;
			bool isTransparent;
		};

		class MeshRenderingManager
		{
			friend class Graphic::GraphicEngine;

		private:
#if RENERING_TYPE_MESH_SHADER_MATERIAL
			static std::map<Graphic::InstatiatingMesh*, std::map<Graphic::Shader*, std::map<Graphic::Material, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<Graphic::InstatiatingMesh*, std::map<Graphic::Shader*, std::map<Graphic::Material, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;
#elif RENERING_TYPE_SHADER_MATERIAL_MESH
			static std::map<Graphic::Shader*, std::map<Graphic::Material, std::map<Graphic::InstatiatingMesh*, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<Graphic::Shader*, std::map<Graphic::Material, std::map<Graphic::InstatiatingMesh*, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;
#elif RENERING_TYPE_SHADER_MESH_MATERIAL
			static std::map<Graphic::Shader*, std::map<Graphic::InstatiatingMesh*,  std::map<Graphic::Material, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<Graphic::Shader*, std::map<Graphic::InstatiatingMesh*,  std::map<Graphic::Material, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;
#endif
			static std::map<Graphic::InstatiatingMesh*, std::queue<MeshRenderData>> _depthQueue;

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