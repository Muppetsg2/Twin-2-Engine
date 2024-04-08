#ifndef _MATERIALS_MANAGER_H_
#define _MATERIALS_MANAGER_H_

#include <graphic/Material.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/TextureManager.h>

namespace Twin2Engine::Manager
{
	class MaterialsManager
	{
		friend class GraphicEngine::Material;

		static std::hash<std::string> stringHash;
		//const static std::unordered_map<size_t, int> typeSizeMap;
		static const std::unordered_map<size_t, int> typeHandleMap;
		static std::list<GraphicEngine::MaterialData*> loadedMaterials;

		static void UnloadMaterial(GraphicEngine::Material& material);
		static GraphicEngine::Material LoadMaterial(const std::string& materialName);
		//static int DetermineSize(const std::string& type);
	public:

		static GraphicEngine::Material GetMaterial(const std::string& name);
		static GraphicEngine::Material CreateMaterial(const std::string& newMaterialName, const std::string& shaderName, const std::vector<std::string>& materialParameters);
	};
}

#endif