#ifndef _MATERIALS_MANAGER_H_
#define _MATERIALS_MANAGER_H_

#include <graphic/Material.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/MaterialParametersBuilder.h>

namespace Twin2Engine::Manager
{
	class SceneManager;
	class PrefabManager;

	class MaterialsManager
	{
		friend class Graphic::Material;
		friend class SceneManager;
		friend class PrefabManager;

		static std::hash<std::string> stringHash;
		static const std::unordered_map<size_t, int> typeHandleMap;
		static std::map<size_t, Graphic::MaterialData*> loadedMaterials;

		static std::map<size_t, std::string> materialsPaths;

		static void UnloadMaterial(size_t managerId);
		static void UnloadMaterial(const std::string& path);
		static Graphic::Material LoadMaterial(const std::string& materialName);
	public:

		static Graphic::Material GetMaterial(size_t managerId);
		static Graphic::Material GetMaterial(const std::string& name);

		static YAML::Node Serialize();
	};
}

#endif