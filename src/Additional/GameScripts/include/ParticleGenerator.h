#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <graphic/Texture2D.h>
#include <graphic/Shader.h>
#include <core/GameObject.h>

#include <vector>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;


// Represents a single particle and its state
struct Particle {
    //glm::vec4 Position;
    glm::vec3 Velocity;
    float     Life;
    //glm::vec4 Color;
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    glm::vec4 startPosition = glm::vec4(0.0f, 2.0f, 0.0f, 1.0f);
    float maxLife = 3.0f;
    bool active = false;

    // constructor
    ParticleGenerator(std::string shaderName, std::string textureName, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH);
    ~ParticleGenerator();
    // update all particles
    void Update();
    // render all particles
    void Draw();
private:
    // state
    std::vector<Particle*> particles;
    std::vector<glm::vec4> particlesPos;
    unsigned int amount;
    Shader* shader;
    Texture2D* texture;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int ParcticleSSBO;

    float horizontalVel = 0.5f;
    float g = 0.5f;
    float verticalVel = 0.5f;
};

#endif