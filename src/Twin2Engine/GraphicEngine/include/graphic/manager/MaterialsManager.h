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
		static std::map<size_t, Graphic::Material*> _loadedMaterials;
		static std::map<size_t, std::string> _materialsPaths;

#if _DEBUG
		// For ImGui
		static bool _fileDialogOpen;
		static ImFileDialogInfo _fileDialogInfo;
#endif

		static void UnloadMaterial(size_t managerId);
		static void UnloadMaterial(const std::string& materialPath);
		static Graphic::Material* LoadMaterial(const std::string& materialPath);
		static void SaveMaterial(const std::string& materialPath, Graphic::Material* mat = nullptr);
	public:

		static bool IsMaterialLoaded(size_t managerId);
		static bool IsMaterialLoaded(const std::string& materialPath);

		static Graphic::Material* GetMaterial(size_t managerId);
		static Graphic::Material* GetMaterial(const std::string& materialPath);

		static std::string GetMaterialName(size_t managerId);
		static std::map<size_t, std::string> GetAllMaterialsNames();

		static void UnloadAll();

		static YAML::Node Serialize();

#if _DEBUG
		static void DrawEditor(bool* p_open);
#endif
	};
}