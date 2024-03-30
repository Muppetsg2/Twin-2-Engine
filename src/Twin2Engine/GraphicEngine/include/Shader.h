#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

#include <ShaderManager.h>

namespace Twin2Engine::GraphicEngine
{
    class Shader
    {
    private:
        friend class ShaderManager;

        //Shader(const GLchar* shaderPath);
        unsigned int shaderProgramID;
        unsigned int _instanceDataSSBO;
        unsigned int _materialIndexSSBO;
        unsigned int _materialInputUBO;
    public:
        // ID program object  
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
        // funkcje operuj¹ce na uniformach  
        void setBool(const std::string& name, bool value) const;

        void setInt(const std::string& name, int value) const;

        void setFloat(const std::string& name, float value) const;

        void setVec4(const std::string& name, float* value) const;

        void setVec3(const std::string& name, float* value) const;

        void setMat4(const std::string& name, glm::mat4& value) const;

        GLuint GetInstanceDataSSBO() const;
        GLuint GetMaterialIndexSSBO() const;
        GLuint GetMaterialInputUBO() const;
    };
}

#endif // !SHADER_H
