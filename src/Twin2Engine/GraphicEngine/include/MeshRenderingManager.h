#ifndef _MESH_RENDERING_MANAGER_H_
#define _MESH_RENDERING_MANAGER_H_

#include <InstatiatingMesh.h>
#include <Material.h>
#include <MeshRenderer.h>
#include <Shader.h>

#include <unordered_map>
#include <map>
#include <vector>

namespace Twin2Engine::GraphicEngine
{
	class Material;
	class MeshRenderer;
	class InstatiatingMesh;
	class Shader;

	struct InstanceData
	{
		glm::mat4 transformMatrix;
		unsigned int materialInputId;
	};
	
	class MeshRenderingManager
	{
		static std::map<InstatiatingMesh*, std::map<Shader*, std::map<Material, std::vector<MeshRenderer*>>>>  renderingData;
	
	public:
		static void Register(MeshRenderer* meshRenderer);
		static void Unregister(MeshRenderer* meshRenderer);
	
		static void Render(const glm::mat4& projectionView);
	};
}


#endif