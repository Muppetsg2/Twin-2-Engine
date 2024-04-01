#ifndef _MESH_RENDERING_MANAGER_H_
#define _MESH_RENDERING_MANAGER_H_

#include <graphic/InstatiatingMesh.h>
#include <graphic/Material.h>
#include <graphic/Shader.h>

namespace Twin2Engine::GraphicEngine
{
	class GraphicEngine;
	class Material;
	class InstatiatingMesh;
	class Shader;

	struct InstanceData
	{
		glm::mat4 transformMatrix;
		unsigned int materialInputId;
	};

	struct MeshRenderData {
		std::vector<InstatiatingMesh*> meshes;
		std::vector<Material> materials;
		glm::mat4 transform;
	};
	
	class MeshRenderingManager
	{
	private:
		static std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>  _renderQueue;
		
		static void Render();
	public:
		static void Render(MeshRenderData meshData);

		friend class GraphicEngine;
	};
}


#endif