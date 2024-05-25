#pragma once

#include <core/Scene.h>

namespace Twin2Engine::Manager {
	class PrefabManager;
	class SceneManager;
}

namespace Twin2Engine::Core {
	class Prefab : public Scene {
	protected:
		size_t _id;
		YAML::Node _rootObject;

	public:
		Prefab(size_t id);
		virtual ~Prefab() = default;

		size_t GetId() const;

		void SetRootObject(const YAML::Node& rootObjectNode);

		void Deserialize(const YAML::Node& prefabNode) override;

		// TODO: DODAC EDYTOR PODOBNY DO SCENMANAGERA

		friend class Manager::PrefabManager;
		friend class Manager::SceneManager;
	};
}