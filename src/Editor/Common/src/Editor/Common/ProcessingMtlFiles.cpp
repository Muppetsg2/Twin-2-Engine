#include <Editor/Common/ProcessingMtlFiles.h>

using namespace Editor::Common;

namespace fs = std::filesystem;
using namespace glm;
using namespace std;

//struct MtlFileMaterial
//{
//    string name;
//    vec3 Ka;
//    vec3 Ks;
//    vec3 Ks;
//    vec3 Ke;
//    float Ns;
//    vector<string> map_Ka;
//    vector<string> map_Kd;
//    vector<string> map_Ks;
//    vector<string> map_Ns;
//};

void Editor::Common::processMTLFiles(const fs::path& inputDir, const fs::path& outputDir)
{
    for (const auto& entry : fs::recursive_directory_iterator(inputDir)) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".obj") 
        {
            fs::path dstDirectory = (outputDir / fs::relative(inputDir, entry.path())).parent_path();

            parseMaterial(entry.path(), dstDirectory);
        }
    }
}

void Editor::Common::parseMaterial(const fs::path& filePath, const fs::path& dstDirectory) 
{
    Assimp::Importer importer;
    //const aiScene* scene = importer.ReadFile("path/to/your/model.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
    const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_FlipUVs);

    if (!scene || !scene->HasMaterials()) 
    {
        SPDLOG_INFO("Failed to load the model or it has no materials.");
        return;
    }

    // Iterate through materials and print their texture maps
    for (unsigned int i = 1; i < scene->mNumMaterials; ++i)
    {
        const aiMaterial* material = scene->mMaterials[i];
        SPDLOG_INFO("Loading material: {} from {}.", i, filePath.string());
        saveMaterialFromMTLFiles(material, dstDirectory, filePath);
    }
}


void Editor::Common::saveMaterialFromMTLFiles(const aiMaterial* material, const fs::path& dstDirectory, const fs::path& srcModel)
{
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "material";
    emitter << YAML::Value << YAML::BeginMap;

    //YAML::Node materialNode;

    aiString materialName;
    if (material->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS) 
    {
        emitter << YAML::Key << "name";
        emitter << YAML::Value << materialName.C_Str();

        //materialNode["name"] = materialName.C_Str();
    }
    else
    {
        SPDLOG_ERROR("Parse material in .mtl file of {} doesn't have name!", srcModel.string());
        return;
    }

    SPDLOG_INFO("Loaded material name: {}", materialName.C_Str());
    
    fs::create_directories(dstDirectory);

    fs::path outputPath = dstDirectory / fs::path(srcModel.stem().string() + "_" + materialName.C_Str() + static_cast<string>(".material"));
    std::ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to create output file: " << outputPath << std::endl;
        return;
    }

    emitter << YAML::Key << "shader";
    emitter << YAML::Value << "origin/Standard";
    emitter << YAML::Key << "parameters";
    emitter << YAML::Value << YAML::BeginSeq;

    aiColor3D ambientColor, diffuseColor, specularColor, emissiveColor;
    float shininess, opacity;


    // Opacity (d)
    if (material->Get(AI_MATKEY_OPACITY, opacity) != AI_SUCCESS) 
    {
        //std::cout << "Opacity (d): " << opacity << "\n";
    }
    else
    {
        opacity = 1.0f;
    }

    // Ambient color (Ka)
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "name";
    emitter << YAML::Value << "ambientColor";
    emitter << YAML::Key << "type";
    emitter << YAML::Value << "vec4";
    emitter << YAML::Key << "value";
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS) 
    {
        emitter << YAML::Value << vector<float>({ ambientColor.r, ambientColor.g, ambientColor.b, opacity });
    }
    else
    {
        emitter << YAML::Value << vector<float>({ 0.f, 0.f, 0.f, opacity });
    }
    emitter << YAML::EndMap;

    // Diffuse color (Kd)
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "name";
    emitter << YAML::Value << "diffuseColor";
    emitter << YAML::Key << "type";
    emitter << YAML::Value << "vec4";
    emitter << YAML::Key << "value";
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) 
    {
        emitter << YAML::Value << vector<float>({ diffuseColor.r, diffuseColor.g, diffuseColor.b, opacity });
    }
    else
    {
        emitter << YAML::Value << vector<float>({ 0.f, 0.f, 0.f, opacity });
    }
    emitter << YAML::EndMap;

    // Specular color (Ks)
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "name";
    emitter << YAML::Value << "specularColor";
    emitter << YAML::Key << "type";
    emitter << YAML::Value << "vec4";
    emitter << YAML::Key << "value";
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
    {
        emitter << YAML::Value << vector<float>({ specularColor.r, specularColor.g, specularColor.b, opacity });
    }
    else
    {
        emitter << YAML::Value << vector<float>({ 0.f, 0.f, 0.f, opacity });
    }
    emitter << YAML::EndMap;

    // Emissive color (Ke)
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "name";
    emitter << YAML::Value << "emissiveColor";
    emitter << YAML::Key << "type";
    emitter << YAML::Value << "vec4";
    emitter << YAML::Key << "value";
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == AI_SUCCESS) 
    {
        emitter << YAML::Value << vector<float>({ emissiveColor.r, emissiveColor.g, emissiveColor.b, opacity });
    }
    else
    {
        emitter << YAML::Value << vector<float>({ 0.f, 0.f, 0.f, opacity });
    }
    emitter << YAML::EndMap;

    // Shininess (Ns)
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "name";
    emitter << YAML::Value << "shininess";
    emitter << YAML::Key << "type";
    emitter << YAML::Value << "float";
    emitter << YAML::Key << "value";
    if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) 
    {
        emitter << YAML::Value << shininess;
    }
    else
    {
        emitter << YAML::Value << 0;
    }
    emitter << YAML::EndMap;

    // Print ambient textures
    saveTextures(material, aiTextureType_AMBIENT, emitter, "ambient");

    // Print diffuse textures
    saveTextures(material, aiTextureType_DIFFUSE, emitter, "diffuse");

    // Print specular textures
    saveTextures(material, aiTextureType_SPECULAR, emitter, "specular");

    // Print bump textures
    saveTextures(material, aiTextureType_NORMALS, emitter, "normals");


    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;

    outputFile << emitter.c_str();

    outputFile.close();
}

void Editor::Common::saveTextures(const aiMaterial* material, aiTextureType textureType, YAML::Emitter& emitter, const string& prefix)
{
    unsigned int numTextures = material->GetTextureCount(textureType);

    emitter << YAML::BeginMap;
    emitter << YAML::Key << "name";
    emitter << YAML::Value << prefix + "TexturesCount";
    emitter << YAML::Key << "type";
    emitter << YAML::Value << "uint";
    emitter << YAML::Key << "value";
    emitter << YAML::Value << numTextures;
    emitter << YAML::EndMap;

    for (unsigned int i = 0; i < numTextures; ++i) {
        aiString texturePath;
        if (material->GetTexture(textureType, i, &texturePath) == AI_SUCCESS)
        {
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "name";
            emitter << YAML::Value << (prefix + std::to_string(i));
            emitter << YAML::Key << "type";
            emitter << YAML::Value << "texture2D";
            emitter << YAML::Key << "value";
            emitter << YAML::Value << texturePath.C_Str();
            emitter << YAML::EndMap;
        }
    }
}