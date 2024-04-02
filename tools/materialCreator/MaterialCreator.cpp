#include <iostream>
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <material_name> <shader_name>" << std::endl;
        return 1;
    }

    std::string materialName = argv[1];
    std::string shaderName = argv[2];
    //std::string parameterFilePath = "C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\tools\\shader_compiler\\bin\\ShadersOrigin/CompiledShaders/" + shaderName + "_parameters.shpa";
    std::string parameterFilePath = "ShadersOrigin/CompiledShaders/" + shaderName + "_parameters.shpa";

    // Read default parameters from the parameter file
    std::ifstream parameterFile(parameterFilePath);
    if (!parameterFile.is_open()) {
        std::cerr << "Failed to open parameter file: " << parameterFilePath << std::endl;
        return 1;
    }

    std::stringstream parameterContent;
    parameterContent << parameterFile.rdbuf();
    parameterFile.close();

    // Parse YAML content of parameters
    YAML::Node parameters = YAML::Load(parameterContent.str());

    // Write YAML file with material parameters
    YAML::Node materialNode;
    materialNode["name"] = materialName;
    materialNode["shader"] = shaderName;
    materialNode["parameters"] = parameters;

    YAML::Node rootNode;
    std::string nodeName = "material"; // Name of the node
    rootNode[nodeName] = materialNode;
    // Write YAML file
    std::ofstream yamlFile("ShadersOrigin/materials/" + materialName + ".material");
    if (!yamlFile.is_open()) {
        std::cerr << "Failed to create YAML file." << std::endl;
        return 1;
    }
    yamlFile << rootNode;
    yamlFile.close();

    std::cout << "Material YAML file created successfully." << std::endl;
    return 0;
}