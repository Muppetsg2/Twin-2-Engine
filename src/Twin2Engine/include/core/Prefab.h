#pragma once

#include <core/Scene.h>

namespace Twin2Engine::Manager {
	class PrefabManager;
}

namespace Twin2Engine::Core {
	class Prefab : public Scene {
	protected:
		YAML::Node _rootObject;

	public:
		Prefab() = default;
		virtual ~Prefab() = default;

		void SetRootObject(const YAML::Node& rootObjectNode);

		friend class Manager::PrefabManager;
	};
}