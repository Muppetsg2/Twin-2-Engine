#pragma once

#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>


class ConfigManager 
{
private:
    static std::string _filePath;
    static YAML::Node _config;
public:
    //ConfigManager(const std::string& filePath) : _filePath(filePath) {}

    static void OpenConfig(const std::string& filePath)
    {
        _filePath = filePath;
    }

    // Read configuration from file
    static bool ReadConfig() {
        try {
            std::ifstream fin(_filePath);
            if (!fin) {
                std::cerr << "Error: Unable to open config file for reading\n";
                return false;
            }

            _config = YAML::Load(fin);
            fin.close();
            return true;
        }
        catch (const YAML::Exception& e) {
            std::cerr << "YAML Exception: " << e.what() << std::endl;
            return false;
        }
    }

    // Write configuration to file
    static bool WriteConfig() {
        try {
            std::ofstream fout(_filePath);
            if (!fout) {
                std::cerr << "Error: Unable to open config file for writing\n";
                return false;
            }

            fout << _config;
            fout.close();
            return true;
        }
        catch (const YAML::Exception& e) {
            std::cerr << "YAML Exception: " << e.what() << std::endl;
            return false;
        }
    }

    // Access specific configuration values
    template<typename T>
    static T GetValue(const std::string& key) {
        return _config[key].as<T>();
    }

    // Set specific configuration values
    template<typename T>
    static void SetValue(const std::string& key, const T& value) {
        _config[key] = value;
    }

};