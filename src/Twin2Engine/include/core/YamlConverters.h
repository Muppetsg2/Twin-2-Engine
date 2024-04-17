#pragma once

#include <LayersData.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>
#include <core/CameraComponent.h>

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

	template<> struct convert<Twin2Engine::GraphicEngine::TextureFormat> {
		using TextureFormat = Twin2Engine::GraphicEngine::TextureFormat;

		static Node encode(const TextureFormat& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, TextureFormat& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (TextureFormat)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Manager::TextureFileFormat> {
		using TextureFileFormat = Twin2Engine::Manager::TextureFileFormat;

		static Node encode(const TextureFileFormat& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, TextureFileFormat& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (TextureFileFormat)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::GraphicEngine::TextureWrapMode> {
		using TextureWrapMode = Twin2Engine::GraphicEngine::TextureWrapMode;

		static Node encode(const TextureWrapMode& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, TextureWrapMode& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (TextureWrapMode)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::GraphicEngine::TextureFilterMode> {
		using TextureFilterMode = Twin2Engine::GraphicEngine::TextureFilterMode;

		static Node encode(const TextureFilterMode& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, TextureFilterMode& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (TextureFilterMode)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Manager::TextureData> {
		using TextureData = Twin2Engine::Manager::TextureData;
		using TextureWrapMode = Twin2Engine::GraphicEngine::TextureWrapMode;
		using TextureFilterMode = Twin2Engine::GraphicEngine::TextureFilterMode;

		static Node encode(const TextureData& rhs) {
			Node node;
			if (rhs.sWrapMode != TextureWrapMode::MIRRORED_REPEAT) node["sWrapMode"] = rhs.sWrapMode;
			if (rhs.tWrapMode != TextureWrapMode::MIRRORED_REPEAT) node["tWrapMode"] = rhs.tWrapMode;
			if (rhs.minFilterMode != TextureFilterMode::NEAREST_MIPMAP_LINEAR) node["minFilterMode"] = rhs.minFilterMode;
			if (rhs.magFilterMode != TextureFilterMode::LINEAR) node["magFilterMode"] = rhs.magFilterMode;
			return node;
		}

		static bool decode(const Node& node, TextureData& rhs) {
			if (!node.IsMap()) return false;
			if (!node["sWrapMode"] && !node["tWrapMode"] && !node["minFilterMode"] && !node["magFilterMode"]) return false;

			rhs.sWrapMode = node["sWrapMode"].as<TextureWrapMode>();
			rhs.tWrapMode = node["tWrapMode"].as<TextureWrapMode>();
			rhs.minFilterMode = node["minFilterMode"].as<TextureFilterMode>();
			rhs.magFilterMode = node["magFilterMode"].as<TextureFilterMode>();
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

	template<> struct convert<CollisionMode> {
		static Node encode(const CollisionMode& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, CollisionMode& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (CollisionMode)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<LayerCollisionFilter> {
		static Node encode(const LayerCollisionFilter& rhs) {
			Node node;
			node.push_back(rhs.DEFAULT);
			node.push_back(rhs.IGNORE_RAYCAST);
			node.push_back(rhs.IGNORE_COLLISION);
			node.push_back(rhs.UI);
			node.push_back(rhs.LAYER_1);
			node.push_back(rhs.LAYER_2);
			node.push_back(rhs.LAYER_3);
			node.push_back(rhs.LAYER_4);
			return node;
		}

		static bool decode(const Node& node, LayerCollisionFilter& rhs) {
			if (!node.IsSequence()) return false;

			rhs.DEFAULT = node[0].as<CollisionMode>();
			rhs.IGNORE_RAYCAST = node[1].as<CollisionMode>();
			rhs.IGNORE_COLLISION = node[2].as<CollisionMode>();
			rhs.UI = node[3].as<CollisionMode>();
			rhs.LAYER_1 = node[4].as<CollisionMode>();
			rhs.LAYER_2 = node[5].as<CollisionMode>();
			rhs.LAYER_3 = node[6].as<CollisionMode>();
			rhs.LAYER_4 = node[7].as<CollisionMode>();
			return true;
		}
	};

	template<> struct convert<Layer> {
		static Node encode(const Layer& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, Layer& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (Layer)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Core::CameraType> {
		using CameraType = Twin2Engine::Core::CameraType;

		static Node encode(const CameraType& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, CameraType& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (CameraType)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<Twin2Engine::Core::RenderResolution> {
		using RenderResolution = Twin2Engine::Core::RenderResolution;

		static Node encode(const RenderResolution& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, RenderResolution& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (RenderResolution)node.as<size_t>();
			return true;
		}
	};
}