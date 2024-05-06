#include <graphic/manager/ShaderManager.h>
#include <graphic/LightingController.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

GLenum ShaderManager::binaryFormat = 1;

std::hash<std::string> ShaderManager::stringHash;
std::list<ShaderManager::ShaderProgramData> ShaderManager::loadedShaders;

Shader* ShaderManager::DepthShader = nullptr;

const std::unordered_map<size_t, int> ShaderManager::shaderTypeMapping
{
    { ShaderManager::stringHash("vert"), GL_VERTEX_SHADER },
    { ShaderManager::stringHash("geom"), GL_GEOMETRY_SHADER },
    { ShaderManager::stringHash("frag"), GL_FRAGMENT_SHADER }
};

unsigned int ShaderManager::LoadShaderProgram(const std::string& shaderName)
{
    size_t strHash = stringHash(shaderName);

    std::list<ShaderProgramData>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData data) { return data.shaderPathHash == strHash; });

    unsigned int shaderProgramID;

    std::string shaderPath = "res/CompiledShaders/" + shaderName + ".shdr";

    if (found == loadedShaders.end())
    {
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

        loadedShaders.push_back({ .shaderPathHash = strHash, .shaderProgramId = shaderProgramID, .useNumber = 1, .shader = new Shader(shaderProgramID)});
    }
    else
    {
        SPDLOG_INFO("Shader already loaded: {}!", shaderPath);
        shaderProgramID = (*found).shaderProgramId;
        (*found).useNumber++;
    }


    return shaderProgramID;
}

void ShaderManager::IncrementUseNumber(int shaderProgramID)
{
    std::list<ShaderProgramData>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramID](ShaderProgramData data) { return data.shaderProgramId == shaderProgramID; });

    (*found).useNumber++;
}

void ShaderManager::UnloadShaderProgram(int shaderProgramID)
{
    std::list<ShaderProgramData>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [shaderProgramID](ShaderProgramData data) { return data.shaderProgramId == shaderProgramID; });

    if (found == loadedShaders.end())
    {
        SPDLOG_ERROR("Error during unloading ShaderProgram!");
        return;
    }

    ShaderProgramData data = *found;

    data.useNumber--;

    if (data.useNumber == 0)
    {
        delete data.shader;
        loadedShaders.erase(found);
    }
}

bool isWhitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

bool isEmptyOrWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), isWhitespace);
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

std::vector<char> ShaderManager::LoadBinarySource(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    SPDLOG_INFO("Trying to load binary file: {}", filePath);
    if (!file.is_open()) {
        SPDLOG_ERROR("Failed to open  binary source file: {}", filePath);
        return std::vector<char>();
    }

    return std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}


GLuint ShaderManager::CompileShaderSPIRV(GLenum type, const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    SPDLOG_INFO("Trying to load binary file: {}", filePath);
    if (!file.is_open()) {
        SPDLOG_ERROR("Failed to open  binary source file: {}", filePath);
        return 0;
    }
    std::vector<char>source ((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // Create shader object and load SPIRV code
    GLuint shaderId = glCreateShader(type);
    SPDLOG_INFO("SPIR-V shader binary format GL_SHADER_BINARY_FORMAT_SPIR_V: {}", GL_SHADER_BINARY_FORMAT_SPIR_V);
    SPDLOG_INFO("SPIR-V shader binary format GL_SHADER_BINARY_FORMAT_SPIR_V_ARB: {}", GL_SHADER_BINARY_FORMAT_SPIR_V_ARB);
    glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, source.data(), source.size());
    
    glSpecializeShader(shaderId, "main", 0, nullptr, nullptr); //Powodem b³êdów jest to ¿e to jest nullem

    if (!CheckShaderCompilationSuccess(shaderId))
    {
        glDeleteShader(shaderId);
        shaderId = 0;
    }

    return shaderId;
}

inline bool ShaderManager::CheckShaderCompilationSuccess(GLuint shaderId)
{
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        SPDLOG_ERROR("{} ERROR::SHADER::COMPILATION_FAILED\n{}", shaderId, infoLog);
    }
    return success;
}

inline void ShaderManager::CheckProgramLinkingSuccess(GLuint programId)
{
    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        SPDLOG_ERROR("{} ERROR::SHADER::LINKING_FAILED\n{}", programId, infoLog);
    }
}

void ShaderManager::Init()
{
    DepthShader = GetShaderProgram("origin/DepthShader");
}

void ShaderManager::UnloadAll()
{
    for (ShaderProgramData data : loadedShaders) {
        delete data.shader;
    }
    DepthShader = nullptr;
    loadedShaders.clear();
}


Shader* ShaderManager::GetShaderProgram(const std::string& shaderName)
{
#if ENTIRE_SHADER_PROGRAM_PRECOMPILATION
    unsigned int shaderProgramId = LoadShaderProgram(shaderName);
#else
    Shader* shader = LoadShaderProgramSHPR(shaderName);
#endif

    return shader;
}

inline Shader* ShaderManager::LoadShaderProgramSHPR(const std::string& shaderName)
{
    size_t strHash = stringHash(shaderName);

    std::list<ShaderProgramData>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData data) { return data.shaderPathHash == strHash; });
    Shader* shader = nullptr;
    if (found == loadedShaders.end())
    {
        GLuint shaderProgram = CreateShaderProgramFromFile(shaderName);

        shader = new Shader(shaderProgram);

        loadedShaders.push_back({ .shaderPathHash = strHash, .shaderProgramId = shaderProgram, .useNumber = 1, .shader = shader });
    }
    else
    {
        SPDLOG_INFO("Shader already created: {}!", shaderName);
        (*found).useNumber++;
        shader = (*found).shader;
    }
    return shader;
}

GLuint ShaderManager::CreateShaderProgramFromFile(const std::string& shaderProgramName)
{

    //SPDLOG_INFO("From File");
    std::string shaderProgramPath = "res/shaders/" + shaderProgramName + ".shpr";
    
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

        GLuint shaderId = CompileShader(shaderTypeMapping.at(extensionHash), LoadShaderSource(SHADERS_ORIGIN_DIRETORY + ("/shaders/" + shaderName)));

        #endif
    
        //SPDLOG_INFO("TU1");
        glAttachShader(shaderProgram, shaderId);
    
        shaderIds.push_back(shaderId);
    }
    
    //SPDLOG_INFO("Before linking");
    glLinkProgram(shaderProgram);
    {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            SPDLOG_ERROR("Error: {}", error);
        }
    }
    CheckProgramLinkingSuccess(shaderProgram);
    
    for (unsigned int id : shaderIds)
    {
        glDeleteShader(id);
    }
    
    return shaderProgram;
}

Shader* ShaderManager::CreateShaderProgram(const std::string& shaderName, const std::string& vertexShader, const std::string& fragmentShader)
{
    size_t strHash = stringHash(shaderName);

    std::list<ShaderProgramData>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [strHash](ShaderProgramData data) { return data.shaderPathHash == strHash; });
    Shader* shader = nullptr;
    if (found == loadedShaders.end())
    {
        unsigned int vertexId = CompileShader(GL_VERTEX_SHADER, LoadShaderSource("res/" + vertexShader));
        unsigned int fragmentId = CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource("res/" + fragmentShader));

        GLuint shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexId);
        glAttachShader(shaderProgram, fragmentId);

        glLinkProgram(shaderProgram);
        CheckProgramLinkingSuccess(shaderProgram);

        glDeleteShader(vertexId);
        glDeleteShader(fragmentId);

        shader = new Shader(shaderProgram);

        loadedShaders.push_back({ .shaderPathHash = strHash, .shaderProgramId = shaderProgram, .useNumber = 1, .shader = shader });
    }
    else
    {
        SPDLOG_INFO("Shader already created: {}!", shaderName);
        (*found).useNumber++;
        shader = (*found).shader;
    }
    return shader;
}

void ShaderManager::UpdateDirShadowMapsTab()
{
    size_t depthShaderHash = stringHash("origin/DepthShader");

    for (auto& SPD : loadedShaders) {
        if (SPD.shaderPathHash != depthShaderHash) {
            LightingController::Instance()->UpdateShadowMapsTab(SPD.shader);
        }
    }
}