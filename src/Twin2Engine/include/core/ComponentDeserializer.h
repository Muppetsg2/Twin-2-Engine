#pragma once

#include <core/GameObject.h>
#include <core/EventHandler.h>

namespace Twin2Engine::Core {
	class ComponentDeserializer {
	private:
		static std::map<size_t, Action<GameObject*, const YAML::Node&>> _deserializers;

	public:
		static void AddDeserializer(const std::string& type, const Action<GameObject*, const YAML::Node&> deselializer);

		static bool HasDeserializer(const std::string& type);

		static Action<GameObject*, const YAML::Node&> GetDeserializer(const std::string& type);

		static void RemoveDeserializer(const std::string& type);
	};
}