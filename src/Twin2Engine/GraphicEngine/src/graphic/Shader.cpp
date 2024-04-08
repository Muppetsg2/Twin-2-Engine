#include <graphic/Shader.h>
#include <graphic/manager/ShaderManager.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Manager;


//Shader::Shader(const GLchar* shaderPath)
//{
//    shaderProgramID = ShaderManager::LoadShaderProgram(shaderPath);
//}

Shader::Shader(const Shader&& shader)
{
    shaderProgramID = shader.shaderProgramID;
    //_instanceDataSSBO = shader._instanceDataSSBO;
    //_materialInputUBO = shader._materialInputUBO;

    ShaderManager::IncrementUseNumber(shaderProgramID);
}

Shader::Shader(const Shader& shader)
{
    shaderProgramID = shader.shaderProgramID;
    //_instanceDataSSBO = shader._instanceDataSSBO;
    //_materialInputUBO = shader._materialInputUBO;

    ShaderManager::IncrementUseNumber(shaderProgramID);
}

Shader::Shader(unsigned int shaderProgramId)
{
    shaderProgramID = shaderProgramId;

    //// SSBO creation
    //glGenBuffers(1, &_instanceDataSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //
    //glGenBuffers(1, &_materialIndexSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //// UBO creation
    //glGenBuffers(1, &_materialInputUBO);
    //glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Shader::~Shader()
{
    ShaderManager::UnloadShaderProgram(shaderProgramID);
}

// aktywuj shader  
void Shader::Use()
{
    glUseProgram(shaderProgramID);
}
// funkcje operuj¹ce na uniformach  
void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}

void Shader::SetUInt(const std::string& name, unsigned int value) const
{
    glUniform1ui(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Shader::GetProgramId() const
{
    return shaderProgramID;
}

glm::mat4 Shader::GetMat4(const std::string& name) const
{
    GLfloat tab[16]{};

    glGetnUniformfv(shaderProgramID, glGetUniformLocation(shaderProgramID, name.c_str()), 16 * sizeof(GLfloat), tab);

    glm::mat4 mat = glm::make_mat4(tab);

    return mat;
}

//GLuint Shader::GetMaterialInputUBO() const
//{
//    return _instanceDataSSBO;
//}
//
//GLuint Shader::GetMaterialIndexSSBO() const
//{
//    return _materialIndexSSBO;
//}
//
//GLuint Shader::GetInstanceDataSSBO() const
//{
//    return _materialInputUBO;
//}