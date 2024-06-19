#pragma once

#include <graphic/GIF.h>

namespace Twin2Engine::Manager {
	class GIFManager {



	public:
		static Graphic::GIF* Load(const std::string& path);

		static void UnloadAll();

		static YAML::Node Serialize();

#if _DEBUG
		static void DrawEditor(bool* p_open);
#endif
	};
}