#include "ShaderManager.h"

#include "../../Twin2Engine/core/ConfigManager.h"



#include <iostream>
;
using namespace GraphicEngine;

GLenum GraphicEngine::ShaderManager::binaryFormat = 1;

std::hash<std::string> GraphicEngine::ShaderManager::stringHash;
std::list<GraphicEngine::ShaderManager::ShaderProgramData*> GraphicEngine::ShaderManager::loadedShaders;

unsigned int GraphicEngine::ShaderManager::LoadShaderProgram(const std::string& shaderPath)
{
    size_t strHash = stringHash(shaderPath);

    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData* data) { return data->shaderPathHash == strHash; });

    int shaderProgramID;

    if (found == loadedShaders.end())
    {
        std::ifstream file(shaderPath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << shaderPath << std::endl;
            return 0;
        }

        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(fileSize);
        if (!file.read(buffer.data(), fileSize)) {
            std::cerr << "Failed to read file: " << shaderPath << std::endl;
            return 0;
        }
        std::vector<char> shaderBinary = buffer;
        if (shaderBinary.empty()) {
            std::cerr << "Failed to read shader program binary" << std::endl;
            return 0;
        }

        shaderProgramID = glCreateProgram();
        if (!shaderProgramID) {
            std::cerr << "Failed to create shader program" << std::endl;
            return 0;
        }

        //glProgramBinary(shaderProgramID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, shaderBinary.data(), shaderBinary.size());
        //glProgramBinary(shaderProgramID, GL_PROGRAM_BINARY_FORMAT_MESA, shaderBinary.data(), shaderBinary.size());
        glProgramBinary(shaderProgramID, binaryFormat, shaderBinary.data(), shaderBinary.size());

        GLint success;
        glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgramID, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Error linking shader program: " << infoLog << std::endl;
            glDeleteProgram(shaderProgramID);
            return 0;
        }

        loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = shaderProgramID, .useNumber = 1, .shader = new Shader(shaderProgramID)});
    }
    else
    {
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

void GraphicEngine::ShaderManager::UnloadShaderProgram(int shaderProgramID)
{
    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramID](ShaderProgramData* data) { return data->shaderProgramId == shaderProgramID; });

    if (found == loadedShaders.end())
    {
        std::cerr << "Error during unloading ShaderProgram!\n";
        return;
    }

    ShaderProgramData* data = *found;

    data->useNumber--;

    if (data->useNumber == 0)
    {
        glDeleteProgram(data->shaderProgramId);
        loadedShaders.erase(found);
        delete data;
    }
}

bool isWhitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

bool isEmptyOrWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), isWhitespace);
}


void GraphicEngine::ShaderManager::PrecompileShaders()
{
    std::vector<std::string> originFolders = { "ShadersOrigin" };
    //std::vector<std::string> originFolders = { "C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine"};

    // Search for shader program files
    for (const auto& folder : originFolders) {
        std::filesystem::path path = folder;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.path().extension() == SHADER_PROGRAM_EXTENSION) {
                std::ifstream shaderFile(entry.path());
                if (shaderFile.is_open()) {
                    // Read shader program file
                    std::string shaderProgramName;// , vertexShaderPath, fragmentShaderPath;
                    std::getline(shaderFile, shaderProgramName);
                    //std::getline(shaderFile, vertexShaderPath);
                    //std::getline(shaderFile, fragmentShaderPath);

                    GLuint shaderProgram = glCreateProgram();

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

                        string extension = line.substr(line.size() - 4, 4);

                        unsigned int shaderType;
                        if (extension == "vert")
                        {
                            shaderType = GL_VERTEX_SHADER;
                        }
                        else if (extension == "frag")
                        {
                            shaderType = GL_FRAGMENT_SHADER;
                        }
                        else if (extension == "geom")
                        {
                            shaderType = GL_GEOMETRY_SHADER;
                        }
                        else
                        {
                            std::cerr << "Unrecogniced extension in shader program encountered.\n" << "Path: " << entry.path() << std::endl;
                            return;
                        }

                        std::string shaderSource = LoadShaderSource(folder + "/" + line);

                        unsigned int shaderId = glCreateShader(shaderType);
                        const GLchar* const cstrShaderSource = (const GLchar*)shaderSource.c_str();
                        glShaderSource(shaderId, 1, &cstrShaderSource, NULL);
                        glCompileShader(shaderId);

                        CheckShaderCompilationSuccess(shaderId);

                        glAttachShader(shaderProgram, shaderId);

                        shaderIds.push_back(shaderId);
                    }
                    glLinkProgram(shaderProgram);
                    CheckProgramLinkingSuccess(shaderProgram);

                    for (unsigned int id : shaderIds)
                    {
                        glDeleteShader(id);
                    }

                    if (shaderProgram != 0) {
                        // Get program binary
                        GLint binaryLength;
                        glGetProgramiv(shaderProgram, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
                        std::vector<GLchar> binaryData(binaryLength);

                        glGetProgramBinary(shaderProgram, binaryLength, nullptr, &binaryFormat, binaryData.data());

                        // Save binary to file
                        std::string outputFilePath = folder + std::string("/CompiledShaders/") + shaderProgramName + SHADER_BINARY_EXTENSION;
                        std::ofstream binaryFile;// (outputFilePath, std::ios::binary);
                        try {
                            std::filesystem::create_directories(outputFilePath.substr(0, outputFilePath.find_last_of('/')));
                            // return true;
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            std::cerr << "Failed to create directories: " << e.what() << std::endl;
                            //return false;
                        }
                        binaryFile.open(outputFilePath, std::ios::binary);
                        if (binaryFile.is_open())
                        {
                            //std::cout << "Output file opened\n";
                        }
                        else
                        {

                            std::cout << "Output file not opened\n";
                        }
                        binaryFile.write(binaryData.data(), binaryLength);
                        binaryFile.close();

                        std::cout << "Compiled and saved shader program binary: " << outputFilePath << std::endl;
                        std::cout << "Binary format: " << binaryFormat << std::endl;

                        // Cleanup
                        glDeleteProgram(shaderProgram);
                    }
                }
                else {
                    std::cerr << "Failed to open shader program file: " << entry.path() << std::endl;
                }
            }
        }
    }
}

std::string GraphicEngine::ShaderManager::LoadShaderSource(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint GraphicEngine::ShaderManager::CompileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error compiling shader: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void GraphicEngine::ShaderManager::CheckShaderCompilationSuccess(GLuint shaderId)
{
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << shaderId << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << "\n";
    }
}

void GraphicEngine::ShaderManager::CheckProgramLinkingSuccess(GLuint programId)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }
}

void GraphicEngine::ShaderManager::Init()
{
    //ConfigManager configManager("GameConfig.yaml");
    ConfigManager::OpenConfig("GameConfig.yaml");
    ConfigManager::ReadConfig();

    //std::cout << "BinaryFormat: " << configManager.GetValue<int>("binaryFormat") << std::endl;

    binaryFormat = ConfigManager::GetValue<unsigned int>("binaryFormat");

    GLint numFormats;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
    if (numFormats == 0) {
        std::cerr << "No supported binary formats found" << std::endl;
        //glfwTerminate();
        //return -1;
    }


    std::vector<GLint> formats(numFormats);
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats.data());
    //std::cout << "Supported binary formats:" << std::endl;
    bool correctFormat = false;
    for (GLint format : formats) {
        //std::cout << format << std::endl;
        if (binaryFormat == format)
        {
            correctFormat = true;
            break;
        }
    }

    if (!correctFormat)
    {
        std::cout << "Precompiling shaders" << std::endl;
        PrecompileShaders();
        ConfigManager::SetValue<unsigned int>("binaryFormat", binaryFormat);
        ConfigManager::WriteConfig();
    }

}

void GraphicEngine::ShaderManager::End()
{
}

Shader* GraphicEngine::ShaderManager::GetShaderProgram(const std::string& shaderName)
{
    unsigned int shaderProgramId = LoadShaderProgram(shaderName);

    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramId](ShaderProgramData* data) { return data->shaderProgramId == shaderProgramId; });


    return (*found)->shader;
}
