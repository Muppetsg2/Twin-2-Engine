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

        public:
            unsigned int shaderProgramID;
            // ID program object  
            Shader(const Shader&& shader);
            Shader(const Shader& shader);
            Shader(unsigned int shaderProgramId);

            ~Shader();

            // aktywuj shader  
            void Use();
            GLuint GetProgramId() const;
            // funkcje operuj�ce na uniformach  
            void SetBool(const std::string& name, bool value) const;
            void SetInt(const std::string& name, int value) const;
            void SetUInt(const std::string& name, unsigned int value) const;
            void SetFloat(const std::string& name, float value) const;
            void SetVec2(const std::string& name, const glm::vec2& value) const;
            void SetVec3(const std::string& name, const glm::vec3& value) const;
            void SetVec4(const std::string& name, const glm::vec4& value) const;
            void SetMat4(const std::string& name, glm::mat4& value) const;

            glm::mat4 GetMat4(const std::string& name) const;
        };
    }
}