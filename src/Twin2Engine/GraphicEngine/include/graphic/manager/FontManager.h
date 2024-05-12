#pragma once

#include <map>
#include <graphic/Font.h>

namespace Twin2Engine::Manager {
	class SceneManager;
	class PrefabManager;

	class FontManager {
	private:
		static std::hash<std::string> _hasher;
		static std::map<size_t, GraphicEngine::Font*> _fonts;

		static std::map<size_t, std::string> _fontsPaths;

		// For ImGui
		static bool _fileDialogOpen;
		static ImFileDialogInfo _fileDialogInfo;

		static void UnloadFont(size_t fontId);
		static void UnloadFont(const std::string& fontPath);
	public:
		static GraphicEngine::Font* LoadFont(const std::string& fontPath);

		static GraphicEngine::Font* GetFont(size_t fontId);
		static GraphicEngine::Font* GetFont(const std::string& fontPath);
		static std::string GetFontName(size_t fontId);
		static std::string GetFontName(const std::string& fontPath);

		static std::map<size_t, std::string> GetAllFontsNames();

		static void UnloadAll();

		static YAML::Node Serialize();
		static void DrawEditor(bool* p_open);

		friend class SceneManager;
		friend class PrefabManager;
	};
}