#ifndef _MESH_RENDERING_MANAGER_H_
#define _MESH_RENDERING_MANAGER_H_

#include <InstatiatingMesh.h>
#include <Material.h>
#include <MeshRenderer.h>
#include <Shader.h>

#include <unordered_map>
#include <vector>

namespace GraphicEngine
{
	class MeshRenderer;

	class MeshRenderingManager
	{
		static std::unordered_map<InstatiatingMesh*, std::unordered_map<Shader*, std::unordered_map<Material, std::vector<MeshRenderer*>>>>  renderingData;

	public:
		static void Register(MeshRenderer* meshRenderer);
		static void Unregister(MeshRenderer* meshRenderer);

		static void Render();
	};
}


#endif