#pragma once

#include <map>
#include <graphic/Font.h>

namespace Twin2Engine::Manager {
	class FontManager {
	private:
		static std::map<size_t, GraphicEngine::Font*> _fonts;

	public:
		static GraphicEngine::Font* LoadFont(const std::string& fontPath);
		static void UnloadAll();
	};
}