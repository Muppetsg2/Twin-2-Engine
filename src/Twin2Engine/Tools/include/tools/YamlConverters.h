#pragma once
#include <tools/templates.h>

namespace YAML {
	template<class T, size_t L> struct convert<glm::vec<L, T>> {
		template<class R> using vec_test = std::enable_if_t<Twin2Engine::Tools::is_num_in_range_v<L, 1, 4>, R>;
		static inline const std::string valueNames[4] = { "x", "y", "z", "w" };

		static typename vec_test<Node> encode(const glm::vec<L, T>& rhs) {
			Node node;
			for (size_t i = 0; i < L; ++i) {
				node[valueNames[i]] = rhs[i];
			}
			return node;
		}

		static typename vec_test<bool> decode(const Node& node, glm::vec<L, T>& rhs) {
			if (node.IsMap()) {
				for (size_t i = 0; i < L; ++i) {
					if (!node[valueNames[i]]) return false;

					rhs[i] = node[valueNames[i]].as<T>();
				}

				return true;
			}
			else if (node.IsSequence()) {
				size_t i = 0;
				for (; i < node.size() && i < L; i++)
				{
					rhs[i] = node[i].as<T>();
				}
				for (; i < L; i++)
				{
					rhs[i] = 0;
				}

				return true;
			}
			return false;
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

	template<> struct convert<std::wstring> {
		static Node encode(const std::wstring& rhs) {
			Node node;
			std::string temp;
			for (const wchar_t& wc : rhs) {
				temp += (char)(wc >> 8);
				temp += (char)wc;
			}
			node = temp;
			return node;
		}

		static bool decode(const Node& node, std::wstring& rhs) {
			if (!node.IsScalar()) return false;

			rhs = L"";
			std::string value = node.as<std::string>();

			if (value.size() > 0) {
				size_t i = 0;
				// 0 0 0
				for (i = 0; i < value.size() - 1; i += 2) {
					rhs += ((wchar_t)value[i] << 8) | (wchar_t)value[i + 1];
				}
				if (i == value.size() - 1) {
					rhs += ((wchar_t)value[i] << 8);
				}
			}

			return true;
		}
	};
}