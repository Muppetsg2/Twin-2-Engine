#ifndef _MATERIALS_MANAGER_H_
#define _MATERIALS_MANAGER_H_

#include <graphic/Material.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/TextureManager.h>

namespace Twin2Engine::Manager
{
	class SceneManager;

	class MaterialsManager
	{
		friend class GraphicEngine::Material;
		friend class SceneManager;

		static std::hash<std::string> stringHash;
		static const std::unordered_map<size_t, int> typeHandleMap;
		static std::map<size_t, GraphicEngine::MaterialData*> loadedMaterials;

		static std::map<size_t, std::string> materialsPaths;

		static void UnloadMaterial(size_t managerId);
		static void UnloadMaterial(const std::string& path);
		static GraphicEngine::Material LoadMaterial(const std::string& materialName);
		//static int DetermineSize(const std::string& type);
	public:

		static GraphicEngine::Material GetMaterial(size_t managerId);
		static GraphicEngine::Material GetMaterial(const std::string& name);
		static GraphicEngine::Material CreateMaterial(const std::string& newMaterialName, const std::string& shaderName, const std::vector<std::string>& materialParametersNames, const std::vector<std::string>& textureParametersNames);
	};
}

#endif