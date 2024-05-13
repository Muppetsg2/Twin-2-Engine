#pragma once

#include <map>
#include <graphic/Font.h>

namespace Twin2Engine::Manager {
	class SceneManager;
	class PrefabManager;

	class FontManager {
	private:
		static std::hash<std::string> _hasher;
		static std::map<size_t, Graphic::Font*> _fonts;

		static std::map<size_t, std::string> _fontsPaths;

		static void UnloadFont(size_t fontId);
		static void UnloadFont(const std::string& fontPath);
	public:
		static Graphic::Font* LoadFont(const std::string& fontPath);

		static Graphic::Font* GetFont(size_t fontId);
		static Graphic::Font* GetFont(const std::string& fontPath);

		static void UnloadAll();

		static YAML::Node Serialize();

		friend class SceneManager;
		friend class PrefabManager;
	};
}