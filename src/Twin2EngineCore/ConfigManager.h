#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>



class ConfigManager {
public:
    ConfigManager(const std::string& filePath) : m_filePath(filePath) {}

    // Read configuration from file
    bool readConfig() {
        try {
            std::ifstream fin(m_filePath);
            if (!fin) {
                std::cerr << "Error: Unable to open config file for reading\n";
                return false;
            }

            m_config = YAML::Load(fin);
            fin.close();
            return true;
        }
        catch (const YAML::Exception& e) {
            std::cerr << "YAML Exception: " << e.what() << std::endl;
            return false;
        }
    }

    // Write configuration to file
    bool writeConfig() {
        try {
            std::ofstream fout(m_filePath);
            if (!fout) {
                std::cerr << "Error: Unable to open config file for writing\n";
                return false;
            }

            fout << m_config;
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
    T getValue(const std::string& key) const {
        return m_config[key].as<T>();
    }

    // Set specific configuration values
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        m_config[key] = value;
    }

private:
    std::string m_filePath;
    YAML::Node m_config;
};