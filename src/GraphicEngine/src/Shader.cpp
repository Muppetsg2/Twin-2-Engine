#include "Shader.h"

std::hash<std::string> Shader::stringHash;
std::list<Shader::ShaderProgramData*> Shader::loadedShaders;

static int getShaderProgramById(int shaderProgramId)
{

}

int Shader::loadShaderProgram(const GLchar* shaderPath)
{
    size_t strHash = stringHash(std::string(shaderPath));

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
        glProgramBinary(shaderProgramID, 1, shaderBinary.data(), shaderBinary.size());

        GLint success;
        glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgramID, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Error linking shader program: " << infoLog << std::endl;
            glDeleteProgram(shaderProgramID);
            return 0;
        }

        loadedShaders.push_back(new ShaderProgramData{ .shaderPathHash = strHash, .shaderProgramId = shaderProgramID, .useNumber = 1 });
    }
    else
    {
        shaderProgramID = (*found)->shaderProgramId;
        (*found)->useNumber++;
    }


    return shaderProgramID;
}

void Shader::unloadShaderProgram(int shaderProgramID)
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

void Shader::checkShaderCompilationSuccess(GLuint shaderId)
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

void Shader::checkProgramLinkingSuccess(GLuint programId)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }
}
/*
Shader::Shader(bool create, const GLchar* vertexPath, const GLchar* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // zapewnij by obiekt ifstream móg³ rzucaæ wyj¹tkami  
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // otwórz pliki  
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream; ///
        // zapisz zawartoœæ bufora pliku do strumieni  
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // zamknij uchtywy do plików  
        vShaderFile.close();
        fShaderFile.close();
        // zamieñ strumieñ w ³añcuch znaków  
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    //VERTEX
    unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* const vertexShaderSource = (const GLchar*)vertexCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShaderID);

    //FRAGMENT
    const GLchar* const fragmentShaderSource = (const GLchar*)fragmentCode.c_str();
    unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderID);

    checkShaderCompilationSuccess(vertexShaderID);
    checkShaderCompilationSuccess(fragmentShaderID);
    
    shaderProgramID = glCreateProgram();

    //Do³¹czanie shaderów
    glAttachShader(shaderProgramID, vertexShaderID);
    glAttachShader(shaderProgramID, fragmentShaderID);
    glLinkProgram(shaderProgramID);

    std::cout << "Tutaj2" << std::endl;
    //checkShaderCompilationSuccess(shaderProgramID);
    std::cout << "Tutaj2" << std::endl;
    checkProgramLinkingSuccess(shaderProgramID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);


    GLint binaryLength;
    glGetProgramiv(shaderProgramID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
    std::vector<GLchar> binaryData(binaryLength);
    GLenum binaryFormat;
    glGetProgramBinary(shaderProgramID, binaryLength, nullptr, &binaryFormat, binaryData.data());
    
    
    unsigned int tempId = glCreateProgram();
    glProgramBinary(tempId, 1, binaryData.data(), binaryData.size());
    
    glDeleteProgram(shaderProgramID);
    shaderProgramID = tempId;
    checkProgramLinkingSuccess(shaderProgramID);
}
/**/
Shader::Shader(const GLchar* shaderPath)
{
    shaderProgramID = loadShaderProgram(shaderPath);
}

Shader::Shader(const Shader&& shader)
{
    shaderProgramID = shader.shaderProgramID;

    int id = shaderProgramID;
    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [id](ShaderProgramData* data) { return data->shaderProgramId == id; });

    if (found != loadedShaders.end())
    {
        ShaderProgramData* data = *found;

        data->useNumber++;
    }
}

Shader::Shader(const Shader& shader)
{
    shaderProgramID = shader.shaderProgramID;

    int id = shaderProgramID;
    std::list<ShaderProgramData*>::iterator found = std::find_if(loadedShaders.begin(), loadedShaders.end(), [id](ShaderProgramData* data) { return data->shaderProgramId == id; });

    if (found != loadedShaders.end())
    {
        ShaderProgramData* data = *found;

        data->useNumber++;
    }

}

Shader::~Shader()
{
    unloadShaderProgram(shaderProgramID);
}

// aktywuj shader  
void Shader::use()
{
    glUseProgram(shaderProgramID);
}
// funkcje operuj¹ce na uniformach  
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}

void Shader::setVec4(const std::string& name, float* value) const
{
    glUniform4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, value);
}
void Shader::setVec3(const std::string& name, float* value) const
{
    glUniform3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, value);
}

void Shader::setMat4(const std::string& name, glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
