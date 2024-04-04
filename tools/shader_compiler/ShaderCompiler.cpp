#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <filesystem>
#include "yaml-cpp/yaml.h"

namespace fs = std::filesystem;

// Define the function CompileShaderSPIRV and CheckProgramLinkingSuccess here if they are not provided elsewhere

int main() 
{
    fs::path sourceDir = "ShadersOrigin/shaders";
    fs::path destinationDir = "ShadersOrigin/CompiledShaders";
    for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".shpr") 
        {
            // Read YAML content
            std::ifstream file(entry.path());
            YAML::Node yaml = YAML::Load(file);

            // Extract shader name from YAML
            std::string shaderName = yaml["shaderprogram"]["name"].as<std::string>();

            // Construct destination path
            fs::path destinationPath = destinationDir / (shaderName + ".shpr");

            // Copy file
            fs::copy_file(entry.path(), destinationPath, fs::copy_options::overwrite_existing);

            std::cout << "Copied: " << entry.path().filename() << " to " << destinationPath << std::endl;
        }
    }
    // Directory containing shader programs
    std::string shaderDirectory = "ShadersOrigin/CompiledShaders/";

    // Iterate over files in the directory
    for (const auto& entry : fs::recursive_directory_iterator(shaderDirectory))
    {
        std::string shaderProgramPath = entry.path().string();

        // Check if the file has the .shpr extension
        if (shaderProgramPath.size() >= 5 && shaderProgramPath.substr(shaderProgramPath.size() - 5) == ".shpr")
        {
            std::ifstream fin(shaderProgramPath);
            if (!fin) {
                std::cerr << "Unable to open shader program file for reading. File path: " << shaderProgramPath << std::endl;
                continue;
            }

            YAML::Node fileNode;
            try {
                fileNode = YAML::Load(fin);
                fin.close();
            }
            catch (const YAML::Exception& e) {
                std::cerr << "Exception occurred during reading shader program file: " << shaderProgramPath << ". YAML Exception: " << e.what() << std::endl;
                continue;
            }

            const YAML::Node& shaderProgramNode = fileNode["shaderprogram"];

            std::string shaderProgramName = shaderProgramNode["name"].as<std::string>();

            // For demonstration purposes, let's print the shader program name
            std::cout << "Shader program name: " << shaderProgramName << std::endl;


            YAML::Node rootNode;

            // Assign the specific node with its associated name as a child node
            std::string nodeName = "parameters"; // Name of the node
            rootNode[nodeName] = shaderProgramNode["parameters"];

            // Saving to file parapeters
            std::stringstream yamlStream;
            //yamlStream << "parameters:\n";
            yamlStream << rootNode;



            
            std::string shaderProgramParametersPath = shaderDirectory + shaderProgramName + "_parameters.shpa";
            std::ofstream file(shaderProgramParametersPath);
            
            if (file.is_open()) {
                file << yamlStream.str();
                file.close();
                std::cout << "YAML content saved to file: " << shaderProgramParametersPath << std::endl;
            }
            else {
                std::cerr << "Failed to open file for writing." << std::endl;
                return 1;
            }

            std::cout << "Shader program processed successfully." << std::endl;
        }
    }

    return 0;
}
