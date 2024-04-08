#ifndef _MESH_RENDERING_MANAGER_H_
#define _MESH_RENDERING_MANAGER_H_

#include <graphic/InstatiatingMesh.h>
#include <graphic/Material.h>
#include <graphic/Shader.h>

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
		};

		class MeshRenderingManager
		{
		private:
			static std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _renderQueue;
			static std::map<GraphicEngine::InstatiatingMesh*, std::map<GraphicEngine::Shader*, std::map<GraphicEngine::Material, std::queue<MeshRenderData>>>>  _depthMapRenderQueue;

			static void Render();
			static void RenderDepthMap();
		public:
			static void Render(MeshRenderData meshData);

			friend class GraphicEngine::GraphicEngineManager;
		};
	}
}


#endif