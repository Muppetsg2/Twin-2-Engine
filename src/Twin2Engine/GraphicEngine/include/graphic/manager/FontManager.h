#pragma once

#include <map>
#include <graphic/Font.h>

namespace Twin2Engine::Manager {
	class SceneManager;

	class FontManager {
	private:
		static std::hash<std::string> _hasher;
		static std::map<size_t, GraphicEngine::Font*> _fonts;

		static std::map < size_t, std::string> _fontsPaths;

		static void UnloadFont(size_t fontId);
		static void UnloadFont(const std::string& fontPath);
	public:
		static GraphicEngine::Font* LoadFont(const std::string& fontPath);

		static GraphicEngine::Font* GetFont(size_t fontId);
		static GraphicEngine::Font* GetFont(const std::string& fontPath);

		static void UnloadAll();

		friend class SceneManager;
	};
}