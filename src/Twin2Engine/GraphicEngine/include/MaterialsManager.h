#ifndef _MATERIALS_MANAGER_H_
#define _MATERIALS_MANAGER_H_

#include <Material.h>
#include <ShaderManager.h>

namespace Twin2Engine::GraphicEngine
{
	class Material;
	struct MaterialData;

	class MaterialsManager
	{
		friend class Material;

		static std::hash<std::string> stringHash;
		//const static std::unordered_map<size_t, int> typeSizeMap;
		static const std::unordered_map<size_t, int> typeHandleMap;
		static std::list<MaterialData*> loadedMaterials;

		static void UnloadMaterial(Material& material);
		static Material LoadMaterial(const string& materialName);
		//static int DetermineSize(const std::string& type);
	public:

		static Material GetMaterial(const std::string& name);
		static Material CreateMaterial(const std::string& newMaterialName, const std::string& shaderName, const std::vector<string>& materialParameters);
	};
}

#endif