#pragma once

namespace YAML {
	template<> struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {

			if (node.IsMap())
			{
				if (!node["x"] || !node["y"]) return false;

				rhs.x = node["x"].as<float>();
				rhs.y = node["y"].as<float>();
			}
			else if (node.IsSequence())
			{
				int i = 0;
				for (; i < node.size() && i < 2; i++)
				{
					rhs[i] = node[i].as<float>();
				}
				for (; i < 2; i++)
				{
					rhs[i] = 0;
				}
			}
			else
			{
				return false;
			}
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

			if (node.IsMap())
			{
				if (!node["x"] || !node["y"] || !node["z"]) return false;

				rhs.x = node["x"].as<float>();
				rhs.y = node["y"].as<float>();
				rhs.z = node["z"].as<float>();
			}
			else if (node.IsSequence())
			{
				int i = 0;
				for (; i < node.size() && i < 3; i++)
				{
					rhs[i] = node[i].as<float>();
				}
				for (; i < 3; i++)
				{
					rhs[i] = 0;
				}
			}
			else
			{
				return false;
			}
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

			if (node.IsMap())
			{
				if (!node["x"] || !node["y"] || !node["z"] || !node["w"]) return false;

				rhs.x = node["x"].as<float>();
				rhs.y = node["y"].as<float>();
				rhs.z = node["z"].as<float>();
				rhs.w = node["w"].as<float>();
			}
			else if (node.IsSequence())
			{
				int i = 0;
				for (; i < node.size() && i < 4; i++)
				{
					rhs[i] = node[i].as<float>();
				}
				for (; i < 4; i++)
				{
					rhs[i] = 0;
				}
			}
			else
			{
				return false;
			}
			return true;
		}
	};


	template<> struct convert<glm::ivec2> {
		static Node encode(const glm::ivec2& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			return node;
		}

		static bool decode(const Node& node, glm::ivec2& rhs) {
			if (node.IsMap())
			{
				if (!node["x"] || !node["y"]) return false;

				rhs.x = node["x"].as<int>();
				rhs.y = node["y"].as<int>();
			}
			else if (node.IsSequence())
			{
				int i = 0;
				for (; i < node.size() && i < 2; i++)
				{
					rhs[i] = node[i].as<int>();
				}
				for (; i < 2; i++)
				{
					rhs[i] = 0;
				}
			}
			else
			{
				return false;
			}
			return true;
		}
	};

	template<> struct convert<glm::ivec3> {
		static Node encode(const glm::ivec3& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["z"] = rhs.z;
			return node;
		}

		static bool decode(const Node& node, glm::ivec3& rhs) {
			if (node.IsMap())
			{
				if (!node["x"] || !node["y"] || !node["z"]) return false;

				rhs.x = node["x"].as<int>();
				rhs.y = node["y"].as<int>();
				rhs.z = node["z"].as<int>();
			}
			else if (node.IsSequence())
			{
				int i = 0;
				for (; i < node.size() && i < 3; i++)
				{
					rhs[i] = node[i].as<int>();
				}
				for (; i < 3; i++)
				{
					rhs[i] = 0;
				}
			}
			else
			{
				return false;
			}
			return true;
		}
	};

	template<> struct convert<glm::ivec4> {
		static Node encode(const glm::ivec4& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["z"] = rhs.z;
			node["w"] = rhs.w;
			return node;
		}

		static bool decode(const Node& node, glm::ivec4& rhs) {
			if (node.IsMap())
			{
				if (!node["x"] || !node["y"] || !node["z"] || !node["w"]) return false;

				rhs.x = node["x"].as<int>();
				rhs.y = node["y"].as<int>();
				rhs.z = node["z"].as<int>();
				rhs.w = node["w"].as<int>();
			}
			else if (node.IsSequence())
			{
				int i = 0;
				for (; i < node.size() && i < 4; i++)
				{
					rhs[i] = node[i].as<int>();
				}
				for (; i < 4; i++)
				{
					rhs[i] = 0;
				}
			}
			else
			{
				return false;
			}
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
}