#include <Editor/Common/MaterialCreator.h>

using namespace std;

void Editor::Common::CreateMaterial(const string& shaderName, const string& materialName)
{
    //std::string parameterFilePath = "C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\tools\\shader_compiler\\bin\\ShadersOrigin/CompiledShaders/" + shaderName + "_parameters.shpa";
    std::string parameterFilePath = "res/shaders/" + shaderName + ".shpr";

    // Read default parameters from the parameter file
    std::ifstream parameterFile(parameterFilePath);
    if (!parameterFile.is_open()) {
        std::cerr << "Failed to open parameter file: " << parameterFilePath << std::endl;
        return;
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
    materialNode["parameters"] = parameters["shaderprogram"]["parameters"];

    YAML::Node rootNode;
    std::string nodeName = "material"; // Name of the node
    rootNode[nodeName] = materialNode;
    // Write YAML file
    std::ofstream yamlFile("res/materials/" + materialName + ".material");
    if (!yamlFile.is_open()) {
        std::cerr << "Failed to create material file." << std::endl;
        return;
    }
    yamlFile << rootNode;
    yamlFile.close();

    std::cout << "Material file created successfully." << std::endl;
}