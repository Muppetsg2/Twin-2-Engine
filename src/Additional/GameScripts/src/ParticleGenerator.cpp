#include <ParticleGenerator.h>
#include <core/Random.h>
#include <core/Time.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/TextureManager.h>
#include <ParticleSystemsController.h>

using namespace Twin2Engine::Core;

ParticleGenerator::ParticleGenerator(const std::string& shaderName, const std::string& textureName, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH, float generationRadius)
    : amount(amount), maxLife(maxLife), generationRadius(generationRadius)
{
    shader = ShaderManager::GetShaderProgram(shaderName);
    if (TextureManager::IsTextureLoaded(textureName)) {
        textureId = TextureManager::GetTexture2D(textureName)->GetId();
    }
    else {
        textureId = TextureManager::LoadTexture2D(textureName)->GetId();
    }

    init(height, distance, time, particleW, particleH);
}

ParticleGenerator::ParticleGenerator(const std::string& shaderName, const GLuint& textureId, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH, float generationRadius)
    : textureId(textureId), amount(amount), maxLife(maxLife), generationRadius(generationRadius)
{
    shader = ShaderManager::GetShaderProgram(shaderName);

    init(height, distance, time, particleW, particleH);
}

void ParticleGenerator::init(float height, float distance, float time, float particleW, float particleH) {
    ParticleSystemsController::Instance()->particlesGenerators.insert(this);
    // set up mesh and attribute properties
    float yValue = 0.5 * glm::sin(0.7853f) * particleH;
    float zValue = 0.5 * glm::cos(0.7853f) * particleH;
    float particle_quad[] = {
       -0.5f * particleW, -yValue,  zValue, 0.0f, 1.0f,
        0.5f * particleW, -yValue,  zValue, 1.0f, 1.0f,
       -0.5f * particleW,  yValue, -zValue, 0.0f, 0.0f,

       -0.5f * particleW,  yValue, -zValue, 0.0f, 0.0f,
        0.5f * particleW, -yValue,  zValue, 1.0f, 1.0f,
        0.5f * particleW,  yValue, -zValue, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    g = -2 * height / (0.25 * time * time);
    verticalVel = 2 * height / (0.5 * time);
    horizontalVel = distance / (time);

    // create this->amount default particle instances
    Particle* particle;
    glm::vec2 vel(0.0f);
    float minVVel = verticalVel - 0.1f * verticalVel;
    float maxVVel = verticalVel + 0.1f * verticalVel;

    glm::vec4 randOffset(0.0f);
    float randAngle = 0.0f;
    float randRadius = 0.0f;

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

        if (generationRadius > 0.01) {
            randAngle = Random::Range<float>(0.0f, 6.2831f);
            randRadius = Random::Range<float>(0.0f, generationRadius);
            randOffset.x = randRadius * glm::cos(randAngle) + particle->Velocity.x * particle->Life;
            randOffset.z = randRadius * glm::sin(randAngle) + particle->Velocity.z * particle->Life;
        }
        randOffset.y = particle->Velocity.y * particle->Life + 0.5f * g * particle->Life * particle->Life;

        particlesPos.push_back(startPosition + randOffset);
    }

    shader->Use();
    shader->SetInt("particleTexture", 0);
    shader->SetVec3("particleEmmiterPos", startPosition);
    shader->SetFloat("maxHeight", height);
    shader->SetVec4("startColor", startColor);
    shader->SetVec4("endColor", endColor);
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

void ParticleGenerator::SetStartPosition(glm::vec3 newPosition)
{
    float yDiff = newPosition.y - startPosition.y;
    startPosition = glm::vec4(newPosition, 1.0f);
    glm::vec4 randOffset(0.0f);
    float randAngle = 0.0f;
    float randRadius = 0.0f;

    for (unsigned int i = 0; i < this->amount; ++i) {
        if (generationRadius > 0.01) {
            randAngle = Random::Range<float>(0.0f, 6.2831f);
            randRadius = Random::Range<float>(0.0f, generationRadius);
            particlesPos[i].x = startPosition.x + randRadius * glm::cos(randAngle);
            particlesPos[i].z = startPosition.z + randRadius * glm::sin(randAngle);
        }
        else {
            particlesPos[i].x = startPosition.x;
            particlesPos[i].z = startPosition.z;
        }
        particlesPos[i].y += yDiff;
    }

    shader->Use();
    shader->SetVec3("particleEmmiterPos", startPosition);
}

void ParticleGenerator::SetStartColor(glm::vec4 newColor)
{
    startColor = newColor;
    shader->Use();
    shader->SetVec4("startColor", startColor);
}

void ParticleGenerator::SetEndColor(glm::vec4 newColor)
{
    endColor = newColor;
    shader->Use();
    shader->SetVec4("endColor", endColor);
}

void ParticleGenerator::Update()
{
    Particle* p;
    float dt = Time::GetDeltaTime();
    if (dt >= 0.1) {
        dt = 0.1;
    }
    glm::vec4 randOffset(0.0f);
    float randAngle = 0.0f;
    float randRadius = 0.0f;

    for (unsigned int i = 0; i < amount; ++i)
    {
        p = particles[i];
        p->Life += dt; // reduce life
        particlesPos[i].x += p->Velocity.x * dt;
        particlesPos[i].y = startPosition.y + p->Velocity.y * p->Life + 0.5f * g * p->Life * p->Life;
        particlesPos[i].z += p->Velocity.z * dt;
        if (p->Life > maxLife)
        {
            if (generationRadius > 0.01) {
                randAngle = Random::Range<float>(0.0f, 6.2831f);
                randRadius = Random::Range<float>(0.0f, generationRadius);
                randOffset.x = randRadius * glm::cos(randAngle);
                randOffset.z = randRadius * glm::sin(randAngle);
                particlesPos[i] = startPosition + randOffset;
            }
            else {
                particlesPos[i] = startPosition;
            }

            p->Life = 0.0f;
        }
    }

    //SPDLOG_INFO("PS y: {}", particlesPos[0].y);
}

// render all particles
void ParticleGenerator::Draw()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticleSystemsController::Instance()->ParcticleSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, amount * sizeof(glm::vec4), particlesPos.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    //glActiveTexture(GL_TEXTURE0 + textureBinded);
    //glBindTexture(GL_TEXTURE_2D, _textures[i]);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}