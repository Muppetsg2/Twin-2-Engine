#pragma once
#include <tools/YamlConverters.h>

namespace Twin2Engine::Core {
	class PlayerPrefs {
	private:
		static const std::string playerPrefsFilePath;

		static bool CheckFileExist() {
			if (FILE* file = fopen(playerPrefsFilePath.c_str(), "r")) {
				fclose(file);
				return true;
			}
			else {
				return false;
			}
		}

		static YAML::Node LoadFile() {
			if (!CheckFileExist()) {
				SaveFile(YAML::Node());
			}
			return YAML::LoadFile(playerPrefsFilePath);
		}

		static void SaveFile(const YAML::Node& fileNode) {
			std::ofstream file{ playerPrefsFilePath };
			file << fileNode;
			file.close();
		}

	public:
		template<class T> static void SetValue(const std::string& name, T value) {
			YAML::Node fileNode = LoadFile();
			fileNode[name] = value;
			SaveFile(fileNode);
		}

		template<class T> static T GetValue(const std::string& name, T defaultValue) {
			YAML::Node fileNode = LoadFile();
			if (fileNode[name]) return fileNode[name].as<T>();
			else return defaultValue;
		}

		template<class T> static T GetValue(const std::string& name) {
			return GetValue(name, T());
		}
	};
}