#include "Shader.h"

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
// konstruktor czyta plik shadera z dysku i tworzy go  
Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath)
{
    std::string vertexCode;
    std::string geometryCode; //////
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream gShaderFile; /////
    std::ifstream fShaderFile;
    // zapewnij by obiekt ifstream móg³ rzucaæ wyj¹tkami  
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); ///
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // otwórz pliki  
        vShaderFile.open(vertexPath);
        gShaderFile.open(geometryPath);/////
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, gShaderStream, fShaderStream; ///
        // zapisz zawartoœæ bufora pliku do strumieni  
        vShaderStream << vShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();///
        fShaderStream << fShaderFile.rdbuf();
        // zamknij uchtywy do plików  
        vShaderFile.close();
        gShaderFile.close();///
        fShaderFile.close();
        // zamieñ strumieñ w ³añcuch znaków  
        vertexCode = vShaderStream.str();
        geometryCode = gShaderStream.str();/////
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

    //GEOMETRY
    const GLchar* const geometryShaderSource = (const GLchar*)geometryCode.c_str();
    unsigned int geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShaderID, 1, &geometryShaderSource, NULL);
    glCompileShader(geometryShaderID);

    checkShaderCompilationSuccess(vertexShaderID);
    checkShaderCompilationSuccess(geometryShaderID);///
    checkShaderCompilationSuccess(fragmentShaderID);

    shaderProgramID = glCreateProgram();

    //Do³¹czanie shaderów
    glAttachShader(shaderProgramID, vertexShaderID);
    glAttachShader(shaderProgramID, geometryShaderID); ///
    glAttachShader(shaderProgramID, fragmentShaderID);
    glLinkProgram(shaderProgramID);

    checkShaderCompilationSuccess(shaderProgramID);


    glDeleteShader(vertexShaderID);
    glDeleteShader(geometryShaderID); ///
    glDeleteShader(fragmentShaderID);
}
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
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

    checkProgramLinkingSuccess(shaderProgramID);
    //checkShaderCompilationSuccess(shaderProgramID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

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

Shader::Shader(const GLchar* shaderPath)
{
    std::ifstream file(shaderPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << shaderPath << std::endl;
        //return {};
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        std::cerr << "Failed to read file: " << shaderPath << std::endl;
        //return {};
    }
    std::vector<char> shaderBinary = buffer;
    if (shaderBinary.empty()) {
        std::cerr << "Failed to read shader program binary" << std::endl;
        //return -1;
    }

    shaderProgramID = glCreateProgram();
    if (!shaderProgramID) {
        std::cerr << "Failed to create shader program" << std::endl;
        //return 0;
    }

    //glProgramBinary(shaderProgramID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, shaderBinary.data(), shaderBinary.size());
    //glProgramBinary(shaderProgramID, GL_PROGRAM_BINARY_FORMAT_MESA, shaderBinary.data(), shaderBinary.size());
    glProgramBinary(shaderProgramID, 1, shaderBinary.data(), shaderBinary.size());
    std::cout << "Error: " << glGetError() << std::endl;
    //checkShaderCompilationSuccess(shaderProgramID);
    //checkProgramLinkingSuccess(shaderProgramID);
    //glLinkProgram(shaderProgramID);
    GLint success;
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgramID, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
        glDeleteProgram(shaderProgramID);
        //return 0;
    }
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
