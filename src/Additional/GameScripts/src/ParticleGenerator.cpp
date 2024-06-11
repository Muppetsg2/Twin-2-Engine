#include <ParticleGenerator.h>
#include <core/Random.h>
#include <core/Time.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/TextureManager.h>
#include <ParticleSystemsController.h>

using namespace Twin2Engine::Core;

ParticleGenerator::ParticleGenerator(std::string shaderName, std::string textureName, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH)
    : amount(amount), maxLife(maxLife)
{
    shader = ShaderManager::GetShaderProgram(shaderName);
    if (TextureManager::IsTextureLoaded(textureName)) {
        texture = TextureManager::GetTexture2D(textureName);
    }
    else {
        texture = TextureManager::LoadTexture2D(textureName);
    }
    ParticleSystemsController::Instance()->particlesGenerators.insert(this);


    // set up mesh and attribute properties
    float yValue = 0.5 * glm::sin(0.7853f) * particleH;
    float zValue = 0.5 * glm::cos(0.7853f) * particleH;
    float particle_quad[] = {
       -0.5f * particleW, -yValue,  zValue, 1.0f, 0.0f, 0.0f,
        0.5f * particleW, -yValue,  zValue, 1.0f, 1.0f, 0.0f,
       -0.5f * particleW,  yValue, -zValue, 1.0f, 0.0f, 1.0f,
                                    
       -0.5f * particleW,  yValue, -zValue, 1.0f, 0.0f, 1.0f,
        0.5f * particleW, -yValue,  zValue, 1.0f, 1.0f, 0.0f,
        0.5f * particleW,  yValue, -zValue, 1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
    glBindVertexArray(0);

    g = -2 * height / (0.25 * time * time);
    verticalVel = 2 * height / (0.5 * time);
    horizontalVel = distance / (time);

    // create this->amount default particle instances
    Particle* particle;
    glm::vec2 vel(0.0f);
    float minVVel = verticalVel - 0.1f * verticalVel;
    float maxVVel = verticalVel + 0.1f * verticalVel;
    for (unsigned int i = 0; i < this->amount; ++i) {
        vel.x = Random::Range<float>(-1.0f, 1.0f);
        vel.y = Random::Range<float>(-1.0f, 1.0f);
        vel = glm::normalize(vel) * horizontalVel;

        particle = new Particle();
        //particle->Position = startPosition;
        particle->Velocity.x = vel.x;
        particle->Velocity.y = Random::Range<float>(minVVel, maxVVel);
        particle->Velocity.z = vel.y;
        particle->Life = Random::Range<float>(0, maxLife);
        particles.push_back(particle);
        particlesPos.push_back(startPosition);
    }

    shader->Use();
    shader->SetInt("particleTexture", 0);

    glGenBuffers(1, &ParcticleSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParcticleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, amount * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ParcticleSSBO);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, amount * sizeof(glm::vec4), particlesPos.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

ParticleGenerator::~ParticleGenerator()
{
    ParticleSystemsController::Instance()->particlesGenerators.erase(this);
    Particle* p;
    while (particles.size() > 0) {
        p = particles.back();
        particles.pop_back();
        delete p;
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleGenerator::Update()
{
    Particle* p;
    float dt = Time::GetDeltaTime();

    for (unsigned int i = 0; i < amount; ++i)
    {
        p = particles[i];
        p->Life += dt; // reduce life
        particlesPos[i].x = startPosition.x + p->Velocity.x * p->Life;
        particlesPos[i].y = startPosition.y + p->Velocity.y * p->Life + 0.5f * g * p->Life * p->Life;
        particlesPos[i].z = startPosition.z + p->Velocity.z * p->Life;
        if (p->Life > maxLife)
        {
            particlesPos[i] = startPosition;
            p->Life = 0.0f;
        }
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParcticleSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, amount * sizeof(glm::vec4), particlesPos.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// render all particles
void ParticleGenerator::Draw()
{
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetId());
    //glActiveTexture(GL_TEXTURE0 + textureBinded);
    //glBindTexture(GL_TEXTURE_2D, _textures[i]);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}