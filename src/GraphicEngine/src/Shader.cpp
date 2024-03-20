#include "Shader.h"

using namespace GraphicEngine;


Shader::Shader(const GLchar* shaderPath)
{
    shaderProgramID = ShaderManager::LoadShaderProgram(shaderPath);
}

Shader::Shader(const Shader&& shader)
{
    shaderProgramID = shader.shaderProgramID;

    ShaderManager::IncrementUseNumber(shaderProgramID);
}

Shader::Shader(const Shader& shader)
{
    shaderProgramID = shader.shaderProgramID;

    ShaderManager::IncrementUseNumber(shaderProgramID);
}

Shader::Shader(unsigned int shaderProgramId)
{
    shaderProgramID = shaderProgramId;
}

Shader::~Shader()
{
    ShaderManager::UnloadShaderProgram(shaderProgramID);
}

// aktywuj shader  
void Shader::use()
{
    glUseProgram(shaderProgramID);
}
// funkcje operuj�ce na uniformach  
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
