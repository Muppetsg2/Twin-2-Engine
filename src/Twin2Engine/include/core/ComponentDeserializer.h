#pragma once

#include <core/GameObject.h>
#include <core/EventHandler.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>

namespace Twin2Engine::Core {
	class ComponentDeserializer {
	private:
		using ComponentFunc = Func<Component*>;
		using DeserializeAction = Action<Component*, const YAML::Node&>;
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

namespace YAML {
	template<> struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsMap()) return false;
			if (!node["x"] || !node["y"]) return false;

			rhs.x = node["x"].as<float>();
			rhs.y = node["y"].as<float>();
			return true;
		}
	};

	template<> struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["z"] = rhs.z;
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsMap()) return false;
			if (!node["x"] || !node["y"] || !node["z"]) return false;

			rhs.x = node["x"].as<float>();
			rhs.y = node["y"].as<float>();
			rhs.z = node["z"].as<float>();
			return true;
		}
	};

	template<> struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["z"] = rhs.z;
			node["w"] = rhs.w;
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsMap()) return false;
			if (!node["x"] || !node["y"] || !node["z"] || !node["w"]) return false;

			rhs.x = node["x"].as<float>();
			rhs.y = node["y"].as<float>();
			rhs.z = node["z"].as<float>();
			rhs.w = node["w"].as<float>();
			return true;
		}
	};

	template<> struct convert<glm::quat> {
		static Node encode(const glm::quat& rhs) {
			Node node;
			node["s"] = rhs.x;
			node["i"] = rhs.y;
			node["j"] = rhs.z;
			node["k"] = rhs.w;
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs) {
			if (!node.IsMap()) return false;
			if (!node["s"] || !node["i"] || !node["j"] || !node["k"]) return false;

			rhs.x = node["s"].as<float>();
			rhs.y = node["i"].as<float>();
			rhs.z = node["j"].as<float>();
			rhs.w = node["k"].as<float>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Manager::TextureData> {
		using TextureData = Twin2Engine::Manager::TextureData;
		using TextureWrapMode = Twin2Engine::GraphicEngine::TextureWrapMode;
		using TextureFilterMode = Twin2Engine::GraphicEngine::TextureFilterMode;

		static Node encode(const TextureData& rhs) {
			Node node;
			if (rhs.sWrapMode != TextureWrapMode::MIRRORED_REPEAT) node["sWrapMode"] = (size_t)rhs.sWrapMode;
			if (rhs.tWrapMode != TextureWrapMode::MIRRORED_REPEAT) node["tWrapMode"] = (size_t)rhs.tWrapMode;
			if (rhs.minFilterMode != TextureFilterMode::NEAREST_MIPMAP_LINEAR) node["minFilterMode"] = (size_t)rhs.minFilterMode;
			if (rhs.magFilterMode != TextureFilterMode::LINEAR) node["magFilterMode"] = (size_t)rhs.magFilterMode;
			return node;
		}

		static bool decode(const Node& node, TextureData& rhs) {
			if (!node.IsMap()) return false;
			if (!node["sWrapMode"] && !node["tWrapMode"] && !node["minFilterMode"] && !node["magFilterMode"]) return false;

			rhs.sWrapMode = (TextureWrapMode)node["sWrapMode"].as<size_t>();
			rhs.tWrapMode = (TextureWrapMode)node["tWrapMode"].as<size_t>();
			rhs.minFilterMode = (TextureFilterMode)node["minFilterMode"].as<size_t>();
			rhs.magFilterMode = (TextureFilterMode)node["magFilterMode"].as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Manager::SpriteData> {
		using SpriteData = Twin2Engine::Manager::SpriteData;

		static Node encode(const SpriteData& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["width"] = rhs.width;
			node["height"] = rhs.height;
			return node;
		}

		static bool decode(const Node& node, SpriteData& rhs) {
			if (!node.IsMap()) return false;

			rhs.x = node["x"].as<uint32_t>();
			rhs.y = node["y"].as<uint32_t>();
			rhs.width = node["width"].as<uint32_t>();
			rhs.height = node["height"].as<uint32_t>();
			return true;
		}
	};
}