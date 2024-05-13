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
		friend class GraphicEngine::Material;
		friend class SceneManager;
		friend class PrefabManager;
	
	private:
		static std::hash<std::string> _stringHash;
		static const std::unordered_map<size_t, int> _typeHandleMap;
		static std::map<size_t, GraphicEngine::MaterialData*> _loadedMaterials;

		// For ImGui
		static bool _fileDialogOpen;
		static ImFileDialogInfo _fileDialogInfo;

		static std::map<size_t, std::string> _materialsPaths;

		static void UnloadMaterial(size_t managerId);
		static void UnloadMaterial(const std::string& path);
		static GraphicEngine::Material LoadMaterial(const std::string& materialName);
		//static int DetermineSize(const std::string& type);
	public:

		static GraphicEngine::Material GetMaterial(size_t managerId);
		static GraphicEngine::Material GetMaterial(const std::string& name);
		//static GraphicEngine::Material CreateMaterial(const std::string& newMaterialName, const std::string& shaderName, const std::vector<std::string>& materialParametersNames, const std::vector<unsigned int>& materialParametersSizes, const std::vector<std::string>& textureParametersNames);

		static std::string GetMaterialName(size_t managerId);
		static std::map<size_t, std::string> GetAllMaterialsNames();

		static YAML::Node Serialize();
		static void DrawEditor(bool* p_open);
	};
}