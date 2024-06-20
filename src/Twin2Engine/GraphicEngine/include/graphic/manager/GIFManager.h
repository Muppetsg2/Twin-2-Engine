#pragma once

#include <graphic/GIF.h>
#include <graphic/manager/TextureManager.h>

namespace Twin2Engine::Manager {
	class SceneManager;
	class PrefabManager;

	class GIFManager {
	private:
		friend class SceneManager;
		friend class PrefabManager;

		static std::hash<std::string> _hasher;
		static std::unordered_map<size_t, Graphic::GIF*> _gifs;
		static std::unordered_map<size_t, std::string> _paths;

#if _DEBUG
		// For ImGui
		static bool _fileDialogOpen;
		static ImFileDialogInfo _fileDialogInfo;
#endif

	public:
		static Graphic::GIF* Load(const std::string& path);

		static Graphic::GIF* Get(size_t id);
		static Graphic::GIF* Get(const std::string& path);

		static void Unload(size_t id);
		static void Unload(const std::string& path);

		static void UnloadAll();

		static bool IsLoaded(size_t id);
		static bool IsLoaded(const std::string& path);

		static std::string GetName(size_t id);
		static std::string GetName(const std::string& path);

#if _DEBUG
		static std::string GetPath(size_t id);
#endif

		static std::map<size_t, std::string> GetAllNames();

		static YAML::Node Serialize();

#if _DEBUG
		static void DrawEditor(bool* p_open);
#endif
	};
}