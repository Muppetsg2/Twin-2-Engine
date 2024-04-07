#include <graphic/manager/ShaderManager.h>

//#include <core/ConfigManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;

GLenum ShaderManager::binaryFormat = 1;

std::hash<std::string> ShaderManager::stringHash;
std::list<ShaderManager::ShaderProgramData*> ShaderManager::loadedShaders;

const Shader* DepthShader = nullptr;

const std::unordered_map<size_t, int> ShaderManager::shaderTypeMapping
{
    { ShaderManager::stringHash("vert"), GL_VERTEX_SHADER },
    { ShaderManager::stringHash("geom"), GL_GEOMETRY_SHADER },
    { ShaderManager::stringHash("frag"), GL_FRAGMENT_SHADER }
};

unsigned int ShaderManager::LoadShaderProgram(const std::string& shaderName)
{
    size_t strHash = stringHash(shaderName);

    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData* data) { return data->shaderPathHash == strHash; });

    int shaderProgramID;

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

        glProgramBinary(shaderProgramID, binaryFormat, shaderBinary.data(), shaderBinary.size());

        GLint success;
        glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgramID, sizeof(infoLog), nullptr, infoLog);
            SPDLOG_ERROR("Error linking shader program: {}", infoLog);
            //std::cerr << "Error linking shader program: " << infoLog << std::endl;
            glDeleteProgram(shaderProgramID);
            return 0;
        }

        loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = shaderProgramID, .useNumber = 1, .shader = new Shader(shaderProgramID)});
    }
    else
    {
        //SPDLOG_INFO("Shader already loaded: {}!", shaderPath); // Dużo razy jest wywoływane
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

void ShaderManager::UnloadShaderProgram(int shaderProgramID)
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


void ShaderManager::PrecompileShaders()
{
    //std::vector<std::string> originFolders = { "ShadersOrigin" };

    // Search for shader program files
    //for (const auto& folder : originFolders)
    {
        std::filesystem::path path = SHADERS_ORIGIN_DIRETORY;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (entry.path().extension() == SHADER_PROGRAM_EXTENSION) 
            {
                string shaderName;
                GLuint shaderProgramId = CreateShaderProgramFromFile(entry.path().string(), shaderName);

                if (shaderProgramId != 0)
                {
                    SaveShaderProgramToFile(shaderProgramId, shaderName);
                
                    // Cleanup
                    glDeleteProgram(shaderProgramId);
                }
            }
        }
    }
}


std::string ShaderManager::LoadShaderSource(const std::string& filePath)
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

GLuint ShaderManager::CompileShader(GLenum type, const std::string& source)
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

bool ShaderManager::CheckShaderCompilationSuccess(GLuint shaderId)
{
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << shaderId << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << "\n";
    }
    return success;
}

void ShaderManager::CheckProgramLinkingSuccess(GLuint programId)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }
}

void ShaderManager::Init()
{
    //ConfigManager configManager("GameConfig.yaml");
    //ConfigManager::OpenConfig("GameConfig.yaml");
    //ConfigManager::ReadConfig();

    //std::cout << "BinaryFormat: " << configManager.GetValue<int>("binaryFormat") << std::endl;

    //binaryFormat = ConfigManager::GetValue<unsigned int>("binaryFormat");

    GLint numFormats;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
    if (numFormats == 0) {
        SPDLOG_ERROR("No supported binary formats found!");
        //std::cerr << "No supported binary formats found" << std::endl;
        //glfwTerminate();
        //return -1;
    }

    DepthShader = GetShaderProgram("origin/DepthShader");

    ///Ustawienie prekompilacji za ka�dym razem gdy� mo�e doj�� do zmiany shader�w a weryfikacja tylko formatu sprawi, i� nowe shadery nie zostan� uwzgl�dnione

    //std::vector<GLint> formats(numFormats);
    //glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats.data());
    ////std::cout << "Supported binary formats:" << std::endl;
    //bool correctFormat = false;
    //for (GLint format : formats) {
    //    //std::cout << format << std::endl;
    //    if (binaryFormat == format)
    //    {
    //        correctFormat = true;
    //        break;
    //    }
    //}

    //if (!correctFormat)
    {
        SPDLOG_INFO("Shaders precompilation started!");
        //std::cout << "Shaders precompilation started" << std::endl;
        PrecompileShaders();
        //ConfigManager::SetValue<unsigned int>("binaryFormat", binaryFormat);
        //ConfigManager::WriteConfig();
        //std::cout << "Shaders precompilation finished" << std::endl;
        SPDLOG_INFO("Shaders precompilation finished!");
    }
    //else
    //{
    //    SPDLOG_INFO("Shaders already precompiled with correct binary format!");
    //}

}

void ShaderManager::End()
{
}


Shader* ShaderManager::GetShaderProgram(const std::string& shaderName)
{
    unsigned int shaderProgramId = LoadShaderProgram(shaderName);

    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramId](ShaderProgramData* data) { return data->shaderProgramId == shaderProgramId; });

    return (*found)->shader;
}

Shader* ShaderManager::CreateShaderProgram(const std::string& shaderProgramName)
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

            loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = (int)shaderProgramId, .useNumber = 1, .shader = shader });
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

GLuint ShaderManager::CreateShaderProgramFromFile(const std::string& shaderProgramName, std::string& shaderName)
{
    string shaderProgramPath = shaderProgramName;

    GLuint shaderProgram = 0;

    std::ifstream shaderFile(shaderProgramPath);

    if (shaderFile.is_open())
    {
        std::getline(shaderFile, shaderName);

        shaderProgram = glCreateProgram();

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

            glAttachShader(shaderProgram, shaderId);

            shaderIds.push_back(shaderId);
        }

        glLinkProgram(shaderProgram);
        CheckProgramLinkingSuccess(shaderProgram);

        for (unsigned int id : shaderIds)
        {
            glDeleteShader(id);
        }
    }
    else 
    {
        SPDLOG_ERROR("Failed to open .shpr file to read: {}", shaderProgramPath);
    }

    return shaderProgram;
}

void ShaderManager::SaveShaderProgramToFile(GLuint shaderProgramId, const std::string& shaderName)
{
    if (shaderProgramId != 0) {
        // Get program binary
        GLint binaryLength;
        glGetProgramiv(shaderProgramId, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

        std::vector<GLchar> binaryData(binaryLength);
        glGetProgramBinary(shaderProgramId, binaryLength, nullptr, &binaryFormat, binaryData.data());

        // Creating folders for saved .shdr file
        std::string outputFilePath = SHADERS_ORIGIN_DIRETORY + ("/CompiledShaders/" + shaderName) + SHADER_BINARY_EXTENSION;
        try 
        {
            std::filesystem::create_directories(outputFilePath.substr(0, outputFilePath.find_last_of('/')));
        }
        catch (const std::filesystem::filesystem_error& e) 
        {
            SPDLOG_ERROR("Failed to create directories: {}", e.what());
            return;
        }

        // Saving to file
        std::ofstream binaryFile(outputFilePath, std::ios::binary);
        if (!binaryFile.is_open())
        {
            SPDLOG_ERROR("SHADER output file not opened: {}", outputFilePath);
            return;
        }
        binaryFile.write(binaryData.data(), binaryLength);
        binaryFile.close();

        SPDLOG_INFO("Compiled and saved shader program binary: {}", outputFilePath);
        SPDLOG_INFO("Binary format: {}", binaryFormat);
    }
}

Shader* ShaderManager::CreateShaderProgram(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader)
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

        loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = (int)shaderProgram, .useNumber = 1, .shader = shader });
    }
    else
    {
        SPDLOG_INFO("Shader already created: {}!", shaderName);
        (*found)->useNumber++;
        shader = (*found)->shader;
    }
    return shader;
}