#include "ShaderManager.h"
#include "ShaderManager.h"

//#include "../../Twin2Engine/core/ConfigManager.h"

#include <core/ConfigManager.h>

#include <spdlog/spdlog.h>



#include <iostream>

using namespace Twin2Engine::GraphicEngine;

GLenum Twin2Engine::GraphicEngine::ShaderManager::binaryFormat = 1;

std::hash<std::string> Twin2Engine::GraphicEngine::ShaderManager::stringHash;
std::list<Twin2Engine::GraphicEngine::ShaderManager::ShaderProgramData*> Twin2Engine::GraphicEngine::ShaderManager::loadedShaders;

const std::unordered_map<size_t, int> ShaderManager::shaderTypeMapping
{
    { ShaderManager::stringHash("vert"), GL_VERTEX_SHADER },
    { ShaderManager::stringHash("geom"), GL_GEOMETRY_SHADER },
    { ShaderManager::stringHash("frag"), GL_FRAGMENT_SHADER }
};

unsigned int Twin2Engine::GraphicEngine::ShaderManager::LoadShaderProgram(const std::string& shaderName)
{
    size_t strHash = stringHash(shaderName);

    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData* data) { return data->shaderPathHash == strHash; });

    unsigned int shaderProgramID;

    //std::string shaderPath = "ShadersOrigin/CompiledShaders/" + shaderName;
    std::string shaderPath = "ShadersOrigin/CompiledShaders/" + shaderName + ".shdr";

    if (found == loadedShaders.end())
    {
        //SPDLOG_INFO("Directory shader: {}!", std::filesystem::current_path().c_str());
        SPDLOG_INFO("Loading shader: {}!", shaderPath);

        std::ifstream file(shaderPath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            SPDLOG_ERROR("Failed to open file: {}!", shaderPath);
            return 0;
        }

        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> shaderBinary(fileSize);
        if (!file.read(shaderBinary.data(), fileSize)) {
            SPDLOG_ERROR("Failed to read file: {}!", shaderPath);
            return 0;
        }
        //std::vector<char> shaderBinary = buffer;
        if (shaderBinary.empty()) {
            SPDLOG_ERROR("Failed to read shader program binary");
            return 0;
        }

        shaderProgramID = glCreateProgram();
        if (!shaderProgramID) {
            SPDLOG_ERROR("Failed to create shader program");
            return 0;
        }

        //std::vector<char> binaryData(6);
        //int len;
        //SPDLOG_INFO("Binary format during loading: {}", binaryFormat);
        //glGetProgramBinary(shaderProgramID, 0, &len, &binaryFormat, binaryData.data());
        //SPDLOG_INFO("Binary format during loading2: {}", binaryFormat);
        //glProgramBinary(shaderProgramID, binaryFormat, shaderBinary.data(), shaderBinary.size());
        //glProgramBinary(shaderProgramID, GL_SPIR_V_BINARY, shaderBinary.data(), shaderBinary.size());
        glProgramBinary(shaderProgramID, GL_SPIR_V_BINARY_ARB, shaderBinary.data(), shaderBinary.size());

        GLint success;
        glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgramID, sizeof(infoLog), nullptr, infoLog);
            SPDLOG_ERROR("Error linking shader program: {}", infoLog);
            glDeleteProgram(shaderProgramID);
            return 0;
        }

        loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = shaderProgramID, .useNumber = 1, .shader = new Shader(shaderProgramID)});
    }
    else
    {
        SPDLOG_INFO("Shader already loaded: {}!", shaderPath);
        shaderProgramID = (*found)->shaderProgramId;
        (*found)->useNumber++;
    }


    return shaderProgramID;
}

void ShaderManager::IncrementUseNumber(int shaderProgramID)
{
    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramID](ShaderProgramData* data) { return data->shaderProgramId == shaderProgramID; });

    (*found)->useNumber++;
}

void Twin2Engine::GraphicEngine::ShaderManager::UnloadShaderProgram(int shaderProgramID)
{
    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramID](ShaderProgramData* data) { return data->shaderProgramId == shaderProgramID; });

    if (found == loadedShaders.end())
    {
        SPDLOG_ERROR("Error during unloading ShaderProgram!");
        //std::cerr << "Error during unloading ShaderProgram!\n";
        return;
    }

    ShaderProgramData* data = *found;

    data->useNumber--;

    if (data->useNumber == 0)
    {
        glDeleteProgram(data->shaderProgramId);
        loadedShaders.erase(found);

        GLuint _instanceDataSSBO = data->shader->GetInstanceDataSSBO();
        glDeleteBuffers(1, &_instanceDataSSBO);
        GLuint _MaterialIndexSSBO = data->shader->GetMaterialIndexSSBO();
        glDeleteBuffers(1, &_MaterialIndexSSBO);
        GLuint _materialInputUBO = data->shader->GetMaterialInputUBO();
        glDeleteBuffers(1, &_materialInputUBO);

        delete data;
    }
}

bool isWhitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

bool isEmptyOrWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), isWhitespace);
}


//void Twin2Engine::GraphicEngine::ShaderManager::PrecompileShaders()
//{
//    //std::vector<std::string> originFolders = { "ShadersOrigin" };
//
//    // Search for shader program files
//    //for (const auto& folder : originFolders)
//    {
//        std::filesystem::path path = SHADERS_ORIGIN_DIRETORY;
//        for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
//        {
//            if (entry.path().extension() == SHADER_PROGRAM_EXTENSION) 
//            {
//                string shaderName;
//                GLuint shaderProgramId = CreateShaderProgramFromFile(entry.path().string());
//
//                if (shaderProgramId != 0)
//                {
//                    SaveShaderProgramToFile(shaderProgramId, shaderName);
//                
//                    // Cleanup
//                    glDeleteProgram(shaderProgramId);
//                }
//            }
//        }
//    }
//}


std::string Twin2Engine::GraphicEngine::ShaderManager::LoadShaderSource(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file) {
        SPDLOG_ERROR("Failed to open shader source file: {}", filePath);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
GLuint Twin2Engine::GraphicEngine::ShaderManager::CompileShader(GLenum type, const std::string& source)
{
    GLuint shaderId = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    if (!CheckShaderCompilationSuccess(shaderId)) 
    {
        glDeleteShader(shaderId);
        shaderId = 0;
    }

    return shaderId;
}

std::vector<char> Twin2Engine::GraphicEngine::ShaderManager::LoadBinarySource(const std::string& filePath)
//std::vector<unsigned int> Twin2Engine::GraphicEngine::ShaderManager::LoadBinarySource(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    SPDLOG_INFO("Trying to load binary file: {}", filePath);
    if (!file.is_open()) {
        SPDLOG_ERROR("Failed to open  binary source file: {}", filePath);
        return std::vector<char>();
        //return std::vector<unsigned int>();
    }

    return std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    //return std::vector<unsigned int>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}


//GLuint Twin2Engine::GraphicEngine::ShaderManager::CompileShaderSPIRV(GLenum type, const std::vector<char>& source)
GLuint Twin2Engine::GraphicEngine::ShaderManager::CompileShaderSPIRV(GLenum type, const string& filePath)
//GLuint Twin2Engine::GraphicEngine::ShaderManager::CompileShaderSPIRV(GLenum type, const std::vector<unsigned int>& source)
{
    std::ifstream file(filePath, std::ios::binary);
    SPDLOG_INFO("Trying to load binary file: {}", filePath);
    if (!file.is_open()) {
        SPDLOG_ERROR("Failed to open  binary source file: {}", filePath);
        //return std::vector<char>();
        //return std::vector<unsigned int>();
        return 0;
    }
    std::vector<char>source ((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // Create shader object and load SPIRV code
    GLuint shaderId = glCreateShader(type);
    SPDLOG_INFO("SPIR-V shader binary format GL_SHADER_BINARY_FORMAT_SPIR_V: {}", GL_SHADER_BINARY_FORMAT_SPIR_V);
    SPDLOG_INFO("SPIR-V shader binary format GL_SHADER_BINARY_FORMAT_SPIR_V_ARB: {}", GL_SHADER_BINARY_FORMAT_SPIR_V_ARB);
    glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, source.data(), source.size());
    //glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, source.data(), source.size() * sizeof(unsigned int));
    //glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V, source.data(), source.size() * sizeof(unsigned int));

    //const unsigned int* ptr = reinterpret_cast<const unsigned int*>(source.data());
    //std::vector<unsigned int> spirv(ptr, ptr + (source.data() / 4));
    //glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, source.data(), source.size() * sizeof(unsigned int));
    const unsigned int indexes[1] = { 0};
    const unsigned int bindinds[1] = { 0};
    //SPDLOG_INFO("Checking glSpecializeShader: {}", (glSpecializeShader == 0));
    //SPDLOG_INFO("Shader ID: {}", shaderId);
    //SPDLOG_INFO("GLAD_GL_VERSION_4_6: {}", GLAD_GL_VERSION_4_6);

    SPDLOG_INFO("TU4");
    //glSpecializeShader(shaderId, "main", 0, 0, 0); //Powodem b³êdów jest to ¿e to jest nullem
    glSpecializeShader(shaderId, "main", 0, nullptr, nullptr); //Powodem b³êdów jest to ¿e to jest nullem

    SPDLOG_INFO("TU2");
    if (!CheckShaderCompilationSuccess(shaderId))
    {
        glDeleteShader(shaderId);
        shaderId = 0;
    }

    return shaderId;
}

bool Twin2Engine::GraphicEngine::ShaderManager::CheckShaderCompilationSuccess(GLuint shaderId)
{
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        SPDLOG_ERROR("{} ERROR::SHADER::COMPILATION_FAILED\n{}", shaderId, infoLog);
        //std::cout << shaderId << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << "\n";
    }
    return success;
}

void Twin2Engine::GraphicEngine::ShaderManager::CheckProgramLinkingSuccess(GLuint programId)
{
    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        SPDLOG_ERROR("{} ERROR::SHADER::LINKING_FAILED\n{}", programId, infoLog);
        //std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }
}

void Twin2Engine::GraphicEngine::ShaderManager::Init()
{
    //ConfigManager configManager("GameConfig.yaml");
    //ConfigManager::OpenConfig("GameConfig.yaml");
    //ConfigManager::ReadConfig();
    //
    ////std::cout << "BinaryFormat: " << configManager.GetValue<int>("binaryFormat") << std::endl;
    //
    //binaryFormat = ConfigManager::GetValue<unsigned int>("binaryFormat");
    //
    //GLint numFormats;
    //glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
    //if (numFormats == 0) {
    //    SPDLOG_ERROR("No supported binary formats found!");
    //    //std::cerr << "No supported binary formats found" << std::endl;
    //    //glfwTerminate();
    //    //return -1;
    //}
    //
    /////Ustawienie prekompilacji za ka¿dym razem gdy¿ mo¿e dojœæ do zmiany shaderów a weryfikacja tylko formatu sprawi, i¿ nowe shadery nie zostan¹ uwzglêdnione
    //
    ////std::vector<GLint> formats(numFormats);
    ////glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats.data());
    //////std::cout << "Supported binary formats:" << std::endl;
    ////bool correctFormat = false;
    ////for (GLint format : formats) {
    ////    //std::cout << format << std::endl;
    ////    if (binaryFormat == format)
    ////    {
    ////        correctFormat = true;
    ////        break;
    ////    }
    ////}
    //
    ////if (!correctFormat)
    //{
    //    SPDLOG_INFO("Shaders precompilation started!");
    //    //std::cout << "Shaders precompilation started" << std::endl;
    //    PrecompileShaders();
    //    ConfigManager::SetValue<unsigned int>("binaryFormat", binaryFormat);
    //    ConfigManager::WriteConfig();
    //    //std::cout << "Shaders precompilation finished" << std::endl;
    //    SPDLOG_INFO("Shaders precompilation finished!");
    //}
    ////else
    ////{
    ////    SPDLOG_INFO("Shaders already precompiled with correct binary format!");
    ////}

}

void Twin2Engine::GraphicEngine::ShaderManager::End()
{
}


Shader* Twin2Engine::GraphicEngine::ShaderManager::GetShaderProgram(const std::string& shaderName)
{
#if ENTIRE_SHADER_PROGRAM_PRECOMPILATION
    unsigned int shaderProgramId = LoadShaderProgram(shaderName);
    //std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramId](ShaderProgramData* data) { return data->shaderProgramId == shaderProgramId; });
    //return (*found)->shader;
#else
    Shader* shader = LoadShaderProgramSHPR(shaderName);
#endif

    return shader;
}

Shader* Twin2Engine::GraphicEngine::ShaderManager::CreateShaderProgram(const std::string& shaderProgramName)
{
    //string shaderName;
    //GLuint shaderProgramId = CreateShaderProgramFromFile(shaderProgramName, shaderName);
    //
    //size_t strHash = stringHash(shaderName);
    //
    //std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData* data) { return data->shaderPathHash == strHash; });
    //
    //Shader* shader;
    //
    //if (found == loadedShaders.end())
    //{
    //    shader = new Shader(shaderProgramId);
    //
    //    loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = (int)shaderProgramId, .useNumber = 1, .shader = shader });
    //}
    //else
    //{
    //    SPDLOG_INFO("Shader already created: {}!", shaderName);
    //    glDeleteProgram(shaderProgramId);
    //    (*found)->useNumber++;
    //    shader = (*found)->shader;
    //}
    //
    //return shader;


    string shaderProgramPath = shaderProgramName;

    GLuint shaderProgramId = 0;

    Shader* shader = nullptr;

    std::ifstream shaderFile(shaderProgramPath);
    if (shaderFile.is_open())
    {
        string shaderName;
        std::getline(shaderFile, shaderName);

        size_t strHash = stringHash(shaderName);
        std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData* data) { return data->shaderPathHash == strHash; });
        
        if (found == loadedShaders.end())
        {

            shaderProgramId = glCreateProgram();

            std::string line;
            std::list<unsigned int> shaderIds;

            while (!shaderFile.eof())
            {
                std::getline(shaderFile, line);

                std::cout << line << std::endl;
                if (isEmptyOrWhitespace(line))
                {
                    continue;
                }

                size_t extensionHash = stringHash(line.substr(line.size() - 4, 4));
                if (!shaderTypeMapping.contains(extensionHash))
                {
                    SPDLOG_ERROR("Unrecogniced extension in shader program encountered. Path: {} in ShaderProgram {}", line, shaderProgramName);
                    return 0;
                }

                GLuint shaderId = CompileShader(shaderTypeMapping.at(extensionHash), LoadShaderSource(SHADERS_ORIGIN_DIRETORY + ("/" + line)));

                glAttachShader(shaderProgramId, shaderId);

                shaderIds.push_back(shaderId);
            }

            glLinkProgram(shaderProgramId);
            CheckProgramLinkingSuccess(shaderProgramId);

            for (unsigned int id : shaderIds)
            {
                glDeleteShader(id);
            }

            shader = new Shader(shaderProgramId);

            loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = shaderProgramId, .useNumber = 1, .shader = shader });
        }
        else
        {
            SPDLOG_INFO("Shader already created: {}!", shaderName);
            (*found)->useNumber++;
            shader = (*found)->shader;
        }

    }
    else
    {
        SPDLOG_ERROR("Failed to open .shpr file to read: {}", shaderProgramPath);
    }

    return shader;
}

inline Shader* Twin2Engine::GraphicEngine::ShaderManager::LoadShaderProgramSHPR(const std::string& shaderName)
{
    size_t strHash = stringHash(shaderName);

    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData* data) { return data->shaderPathHash == strHash; });
    Shader* shader;
    if (found == loadedShaders.end())
    {
        GLuint shaderProgram = CreateShaderProgramFromFile(shaderName);

        shader = new Shader(shaderProgram);

        loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = shaderProgram, .useNumber = 1, .shader = shader });
    }
    else
    {
        SPDLOG_INFO("Shader already created: {}!", shaderName);
        (*found)->useNumber++;
        shader = (*found)->shader;
    }
    return shader;
}

GLuint Twin2Engine::GraphicEngine::ShaderManager::CreateShaderProgramFromFile(const std::string& shaderProgramName)
{
    //string shaderProgramPath = "res/CompiledShaders/" + shaderProgramName + ".shpr";
    //
    //GLuint shaderProgram = 0;
    //
    //std::ifstream shaderFile(shaderProgramPath);
    //
    //string shaderName;
    //if (shaderFile.is_open())
    //{
    //    std::getline(shaderFile, shaderName);
    //
    //    shaderProgram = glCreateProgram();
    //
    //    std::string line;
    //    std::list<unsigned int> shaderIds;
    //
    //    while (!shaderFile.eof())
    //    {
    //        std::getline(shaderFile, line);
    //
    //        //std::cout << line << std::endl;
    //        SPDLOG_INFO("Compiled shader object: {}", line);
    //        if (isEmptyOrWhitespace(line))
    //        {
    //            continue;
    //        }
    //
    //        size_t extensionHash = stringHash(line.substr(line.size() - 4, 4));
    //        if (!shaderTypeMapping.contains(extensionHash))
    //        {
    //            SPDLOG_ERROR("Unrecogniced extension in shader program encountered. Path: {} in ShaderProgram {}", line, shaderProgramName);
    //            return 0;
    //        }
    //
    //        //GLuint shaderId = CompileShader(shaderTypeMapping.at(extensionHash), LoadShaderSource(SHADERS_ORIGIN_DIRETORY + ("/" + line)));
    //        //GLuint shaderId = CompileShaderSPIRV(shaderTypeMapping.at(extensionHash), LoadBinarySource(SHADERS_ORIGIN_DIRETORY + ("/CompiledShaders/SPIRV/" + line)));
    //        GLuint shaderId = CompileShaderSPIRV(shaderTypeMapping.at(extensionHash), SHADERS_ORIGIN_DIRETORY + ("/CompiledShaders/SPIRV/" + line));
    //
    //        SPDLOG_INFO("TU1");
    //        glAttachShader(shaderProgram, shaderId);
    //
    //        shaderIds.push_back(shaderId);
    //    }
    //
    //    SPDLOG_INFO("Before linking");
    //    glLinkProgram(shaderProgram);
    //    CheckProgramLinkingSuccess(shaderProgram);
    //
    //    for (unsigned int id : shaderIds)
    //    {
    //        glDeleteShader(id);
    //    }
    //}
    //else 
    //{
    //    SPDLOG_ERROR("Failed to open .shpr file to read: {}", shaderProgramPath);
    //}
    //
    //return shaderProgram;


    /////
    string shaderProgramPath = "res/CompiledShaders/" + shaderProgramName + ".shpr";
    
    GLuint shaderProgram = 0;
    
    YAML::Node fileNode;
    try {
        std::ifstream fin(shaderProgramPath);
        if (!fin) {
            SPDLOG_ERROR("Unable to open shader program file for reading. File path: {}", shaderProgramPath);
    
            return 0;
        }
    
        fileNode = YAML::Load(fin);
        fin.close();
    }
    catch (const YAML::Exception& e) {
        SPDLOG_ERROR("Exception occured during reading shader program file: {}. YAML Exception: {}", shaderProgramPath, e.what());
        return 0;
    }
    
    SPDLOG_INFO("Loading shader program: {}!", shaderProgramName);
    
    const YAML::Node& shaderProgramNode = fileNode["shaderprogram"];
    
    std::string name = shaderProgramNode["name"].as<std::string>();
    //std::string shader = materialNode["shaders"].as<std::string>();
    
    shaderProgram = glCreateProgram();
    
    std::string shaderName;
    std::list<unsigned int> shaderIds;
    
    for (auto shader : shaderProgramNode["shaders"])
    {
        shaderName = shader.as<std::string>();
    
        size_t extensionHash = stringHash(shaderName.substr(shaderName.size() - 4, 4));
    
        #if SPIRV_COMPILATION

        GLuint shaderId = CompileShaderSPIRV(shaderTypeMapping.at(extensionHash), SHADERS_ORIGIN_DIRETORY + ("/CompiledShaders/SPIRV/" + shaderName));

        #elif NORMAL_SHADERS_CREATION

        GLuint shaderId = CompileShader(shaderTypeMapping.at(extensionHash), SHADERS_ORIGIN_DIRETORY + ("/shaders/" + shaderName));

        #endif
    
        SPDLOG_INFO("TU1");
        glAttachShader(shaderProgram, shaderId);
    
        shaderIds.push_back(shaderId);
    }
    
    SPDLOG_INFO("Before linking");
    glLinkProgram(shaderProgram);
    CheckProgramLinkingSuccess(shaderProgram);
    
    for (unsigned int id : shaderIds)
    {
        glDeleteShader(id);
    }
    
    //// Saving to file parapeters
    //std::stringstream yamlStream;
    //yamlStream << shaderProgramNode["parameters"];
    //
    //string shaderProgramPath = "res/CompiledShaders/" + shaderProgramName + "_parameters.shpa";
    //std::ofstream file(shaderProgramPath);
    //
    //if (file.is_open()) {
    //    file << yamlStream.str();
    //    file.close();
    //    std::cout << "YAML content saved to file: output.yaml" << std::endl;
    //}
    //else {
    //    std::cerr << "Failed to open file for writing." << std::endl;
    //    return 1;
    //}
    
    return shaderProgram;
}

//void Twin2Engine::GraphicEngine::ShaderManager::SaveShaderProgramToFile(GLuint shaderProgramId, const std::string& shaderName)
//{
//    if (shaderProgramId != 0) {
//        // Get program binary
//        GLint binaryLength;
//        glGetProgramiv(shaderProgramId, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
//
//        binaryFormat = GL_SPIR_V_BINARY;
//
//        std::vector<GLchar> binaryData(binaryLength);
//        glGetProgramBinary(shaderProgramId, binaryLength, nullptr, &binaryFormat, binaryData.data());
//        
//        
//        // Creating folders for saved .shdr file
//        std::string outputFilePath = SHADERS_ORIGIN_DIRETORY + ("/CompiledShaders/" + shaderName) + SHADER_BINARY_EXTENSION;
//        try 
//        {
//            std::filesystem::create_directories(outputFilePath.substr(0, outputFilePath.find_last_of('/')));
//        }
//        catch (const std::filesystem::filesystem_error& e) 
//        {
//            SPDLOG_ERROR("Failed to create directories: {}", e.what());
//            return;
//        }
//
//        // Saving to file
//        std::ofstream binaryFile(outputFilePath, std::ios::binary);
//        if (!binaryFile.is_open())
//        {
//            SPDLOG_ERROR("SHADER output file not opened: {}", outputFilePath);
//            return;
//        }
//        binaryFile.write(binaryData.data(), binaryLength);
//        binaryFile.close();
//
//        SPDLOG_INFO("Compiled and saved shader program binary: {}", outputFilePath);
//        SPDLOG_INFO("Binary format: {}", binaryFormat);
//    }
//}

Shader* Twin2Engine::GraphicEngine::ShaderManager::CreateShaderProgram(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader)
{
    size_t strHash = stringHash(shaderName);

    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData* data) { return data->shaderPathHash == strHash; });
    Shader* shader;
    if (found == loadedShaders.end())
    {
        unsigned int vertexId = CompileShader(GL_VERTEX_SHADER, LoadShaderSource("ShadersOrigin/" + vertexShader));
        unsigned int fragmentId = CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource("ShadersOrigin/" + fragmentShader));

        GLuint shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexId);
        glAttachShader(shaderProgram, fragmentId);

        glLinkProgram(shaderProgram);
        CheckProgramLinkingSuccess(shaderProgram);

        glDeleteShader(vertexId);
        glDeleteShader(fragmentId);

        shader = new Shader(shaderProgram);

        loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = shaderProgram, .useNumber = 1, .shader = shader });
    }
    else
    {
        SPDLOG_INFO("Shader already created: {}!", shaderName);
        (*found)->useNumber++;
        shader = (*found)->shader;
    }
    return shader;
}