#pragma once

#include <core/GameObject.h>
#include <tools/EventHandler.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>
#include <physics/LayersData.h>
#include <core/CameraComponent.h>
#include <tools/YamlConverters.h>

namespace Twin2Engine::Core {
	class ComponentDeserializer {
	private:
		using ComponentFunc = Tools::Func<Component*>;
		using DeserializeAction = Tools::Action<Component*, const YAML::Node&>;
		using DeserializePair = std::pair<ComponentFunc, DeserializeAction>;

		static std::hash<std::string> hasher;
		static std::map<size_t, DeserializePair> _deserializerPairs;

		static DeserializePair GetDeserializerPair(const std::string& type);
	public:
		static void AddDeserializer(const std::string& type, const ComponentFunc componentFunc, const DeserializeAction deserializeAction);

		static bool HasDeserializer(const std::string& type);

		static ComponentFunc GetComponentFunc(const std::string& type);
		static DeserializeAction GetDeserializeAction(const std::string& type);

		static void RemoveDeserializer(const std::string& type);
	};
}