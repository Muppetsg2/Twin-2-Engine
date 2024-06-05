#include <graphic/Shader.h>
#include <graphic/manager/ShaderManager.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

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
    glDeleteProgram(shaderProgramID);
    //ShaderManager::UnloadShaderProgram(shaderProgramID);
}

// aktywuj shader  
void Shader::Use()
{
    glUseProgram(shaderProgramID);
}
// funkcje operuj�ce na uniformach  
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

void Shader::SetFloatArray(const std::string& name, float* values, size_t size) const
{
    glUniform1fv(glGetUniformLocation(shaderProgramID, name.c_str()), size, values);
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

void Shader::SetMat3(const std::string& name, glm::mat3& value) const
{
    glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
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