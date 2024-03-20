#pragma once

#include <glad/glad.h>// do³¹cz glad, by móc korzystaæ w wszystkich wymaganych przez OpenGL funkcji</glad>
// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;

class Shader
{
private:
    bool isInitialized = false;

public:
    // ID program object  
    GLuint ID;

    // konstruktor domyslny
    Shader();
    // konstruktor czyta plik vertex shadera i fragment shadera z dysku i tworzy go  
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    // konstruktor czyta plik vertex shadera, geometry shadera i fragment shadera z dysku i tworzy go  
    Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);

    // Destruktor
    virtual ~Shader();

    bool IsInitialized() const;

    // aktywuj shader  
    void use() const;
    // funkcje operuj¹ce na uniformach  
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setUInt(const std::string& name, unsigned int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

    glm::mat4 getMat4(const std::string& name) const;
};