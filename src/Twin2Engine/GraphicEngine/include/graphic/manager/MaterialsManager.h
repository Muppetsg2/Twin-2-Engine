#pragma once

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
	
	private:
		static std::hash<std::string> _stringHash;
		static const std::unordered_map<size_t, int> _typeHandleMap;
		static std::map<size_t, Graphic::MaterialData*> _loadedMaterials;

#if _DEBUG
		// For ImGui
		static bool _fileDialogOpen;
		static ImFileDialogInfo _fileDialogInfo;
#endif

		static std::map<size_t, std::string> _materialsPaths;

		static void UnloadMaterial(size_t managerId);
		static void UnloadMaterial(const std::string& path);
		static Graphic::Material LoadMaterial(const std::string& materialName);
	public:

		static Graphic::Material GetMaterial(size_t managerId);
		static Graphic::Material GetMaterial(const std::string& name);

		static std::string GetMaterialName(size_t managerId);
		static std::map<size_t, std::string> GetAllMaterialsNames();

		static void UnloadAll();

		static YAML::Node Serialize();

#if _DEBUG
		static void DrawEditor(bool* p_open);
#endif
	};
}