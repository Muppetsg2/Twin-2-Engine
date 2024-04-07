#pragma once

namespace Twin2Engine
{
    namespace Manager {
        class ShaderManager;
    }

    namespace GraphicEngine 
    {
        class Shader
        {
        private:
            friend class Manager::ShaderManager;

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
            void Use();
            GLuint GetProgramId() const;
            // funkcje operuj¹ce na uniformach  
            void SetBool(const std::string& name, bool value) const;
            void SetInt(const std::string& name, int value) const;
            void SetUInt(const std::string& name, unsigned int value) const;
            void SetFloat(const std::string& name, float value) const;
            void SetVec2(const std::string& name, const glm::vec2& value) const;
            void SetVec3(const std::string& name, const glm::vec3& value) const;
            void SetVec4(const std::string& name, const glm::vec4& value) const;
            void SetMat4(const std::string& name, glm::mat4& value) const;

            glm::mat4 GetMat4(const std::string& name) const;

            GLuint GetInstanceDataSSBO() const;
            GLuint GetMaterialIndexSSBO() const;
            GLuint GetMaterialInputUBO() const;
        };
    }
}