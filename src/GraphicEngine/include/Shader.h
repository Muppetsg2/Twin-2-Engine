#ifndef SHADER_H
#define SHADER_H


#include <glad/glad.h>// do��cz glad, by m�c korzysta� w wszystkich wymaganych przez OpenGL funkcji</glad>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>
#include <set>
#include <vector>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderManager.h"

namespace GraphicEngine
{
    class Shader
    {
    private:
        friend class ShaderManager;

        Shader(const GLchar* shaderPath);
    public:
        // ID program object  
        unsigned int shaderProgramID;
        Shader(const Shader&& shader);
        Shader(const Shader& shader);
        Shader(unsigned int shaderProgramId);


        // konstruktor czyta plik shadera z dysku i tworzy go  
        //Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);
        //Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
        //Shader(bool create, const GLchar* vertexPath, const GLchar* fragmentPath);


        ~Shader();

        // aktywuj shader  
        void use();
        // funkcje operuj�ce na uniformach  
        void setBool(const std::string& name, bool value) const;

        void setInt(const std::string& name, int value) const;

        void setFloat(const std::string& name, float value) const;

        void setVec4(const std::string& name, float* value) const;

        void setVec3(const std::string& name, float* value) const;

        void setMat4(const std::string& name, glm::mat4& value) const;
    };
}

#endif // !SHADER_H
