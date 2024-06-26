#pragma once
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <graphic/Texture2D.h>
#include <graphic/Shader.h>
#include <core/GameObject.h>

#include <vector>
#include <ParticleGenerator.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class UIParticleGenerator
{
    private:
        bool isFront = false;
    public:
        glm::vec2 startPosition = glm::vec4(0.0f, 2.0f, 0.0f, 1.0f);
        glm::vec4 startColor = glm::vec4(1.0f);
        glm::vec4 endColor = glm::vec4(1.0f);
        float maxLife = 3.0f;
        float generationRadius = 0.0f;
        GLuint textureId;
        Shader* shader;
        bool active = false;
    
        // constructor
        UIParticleGenerator(const std::string& shaderName, const std::string& textureName, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH, float generationRadius, bool front);
        UIParticleGenerator(const std::string& shaderName, const GLuint& textureId, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH, float generationRadius, bool front);
        ~UIParticleGenerator();
        // update all particles
        void SetStartPosition(glm::vec2 newPosition);
        void SetStartColor(glm::vec4 newColor);
        void SetEndColor(glm::vec4 newColor);
        void Update();
        // render all particles
        void Draw();
    private:
        // Represents a single particle and its state
        struct Particle {
            //glm::vec4 Position;
            glm::vec3 Velocity;
            float     Life;
            //glm::vec4 Color;
        };
        // state
        std::vector<Particle*> particles;
        std::vector<glm::vec2> particlesPos;
        unsigned int amount;
        unsigned int VAO;
        unsigned int VBO;
    
        float horizontalVel = 0.5f;
        float g = 0.5f;
        float verticalVel = 0.5f;
    
        void init(float height, float distance, float time, float particleW, float particleH);
};