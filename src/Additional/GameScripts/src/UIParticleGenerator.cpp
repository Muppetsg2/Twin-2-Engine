#include <ParticleGenerator.h>
#include <core/Random.h>
#include <core/Time.h>
#include <graphic/Window.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/TextureManager.h>
#include <ParticleSystemsController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

UIParticleGenerator::UIParticleGenerator(const std::string& shaderName, const std::string& textureName, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH, float generationRadius, bool front)
    : amount(amount), maxLife(maxLife), generationRadius(generationRadius)
{
    shader = ShaderManager::GetShaderProgram(shaderName);
    if (TextureManager::IsTextureLoaded(textureName)) {
        textureId = TextureManager::GetTexture2D(textureName)->GetId();
    }
    else {
        textureId = TextureManager::LoadTexture2D(textureName)->GetId();
    }

    isFront = front;
    if (front) {
        ParticleSystemsController::Instance()->UIParticlesGeneratorsFront.insert(this);
    }
    else {
        ParticleSystemsController::Instance()->UIParticlesGeneratorsBack.insert(this);
    }

    init(height, distance, time, particleW, particleH);
}

UIParticleGenerator::UIParticleGenerator(const std::string& shaderName, const GLuint& textureId, unsigned int amount, float height, float distance, float time, float maxLife, float particleW, float particleH, float generationRadius, bool front)
    : textureId(textureId), amount(amount), maxLife(maxLife), generationRadius(generationRadius)
{
    shader = ShaderManager::GetShaderProgram(shaderName);

    isFront = front;
    if (front) {
        ParticleSystemsController::Instance()->UIParticlesGeneratorsFront.insert(this);
    }
    else {
        ParticleSystemsController::Instance()->UIParticlesGeneratorsBack.insert(this);
    }

    init(height, distance, time, particleW, particleH);
}

void UIParticleGenerator::init(float height, float distance, float time, float particleW, float particleH) {

    glm::ivec2 wSize = Window::GetInstance()->GetContentSize();
    float w = particleW / (0.5f * wSize.x);
    float h = particleH / (0.5f * wSize.y);
    float particle_quad[] = {
       -0.5f * w, -0.5f * h, 0.0f, 1.0f,
        0.5f * w, -0.5f * h, 1.0f, 1.0f,
       -0.5f * w,  0.5f * h, 0.0f, 0.0f,
                            
       -0.5f * w,  0.5f * h, 0.0f, 0.0f,
        0.5f * w, -0.5f * h, 1.0f, 1.0f,
        0.5f * w,  0.5f * h, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    g = -2 * height / (0.25 * time * time) / (wSize.y * 0.5f);
    verticalVel = 2 * height / (0.5 * time) / (wSize.y * 0.5f);
    horizontalVel = distance / (time) / (wSize.x * 0.5f);

    // create this->amount default particle instances
    Particle* particle;
    glm::vec2 vel(0.0f);
    float minVVel = verticalVel - 0.1f * verticalVel;
    float maxVVel = verticalVel + 0.1f * verticalVel;

    glm::vec2 randOffset(0.0f);

    for (unsigned int i = 0; i < amount; ++i) {
        vel.x = Random::Range<float>(-1.0f, 1.0f);
        vel = glm::normalize(vel) * horizontalVel;

        particle = new Particle();
        //particle->Position = startPosition;
        particle->Velocity.x = vel.x;
        particle->Velocity.y = Random::Range<float>(minVVel, maxVVel);
        particle->Life = Random::Range<float>(0, maxLife);
        particles.push_back(particle);

        if (generationRadius > 0.01) {
            randOffset.x = Random::Range<float>(-generationRadius, generationRadius) / (wSize.x * 0.5f);
        }
        randOffset.y = particle->Velocity.y * particle->Life + 0.5f * g * particle->Life * particle->Life;

        particlesPos.push_back(startPosition + randOffset);
    }

    shader->Use();
    shader->SetInt("particleTexture", 0);
    shader->SetVec3("particleEmmiterPos", glm::vec3(startPosition, 0.0f));
    shader->SetFloat("maxHeight", height);
    shader->SetVec4("startColor", startColor);
    shader->SetVec4("endColor", endColor);
}

UIParticleGenerator::~UIParticleGenerator()
{
    if (isFront) {
        ParticleSystemsController::Instance()->UIParticlesGeneratorsFront.erase(this);
    }
    else {
        ParticleSystemsController::Instance()->UIParticlesGeneratorsBack.erase(this);
    }

    Particle* p;
    while (particles.size() > 0) {
        p = particles.back();
        particles.pop_back();
        delete p;
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void UIParticleGenerator::SetStartPosition(glm::vec2 newPosition)
{
    glm::ivec2 wSize = Window::GetInstance()->GetContentSize();
    float yDiff = newPosition.y / (wSize.y * 0.5f) - startPosition.y;
    startPosition = newPosition / (wSize.y * 0.5f);

    for (unsigned int i = 0; i < this->amount; ++i) {
        if (generationRadius > 0.01) {
            particlesPos[i].x = startPosition.x + Random::Range<float>(-generationRadius, generationRadius) / (wSize.x * 0.5f);
        }
        else {
            particlesPos[i].x = startPosition.x;
        }
        particlesPos[i].y += yDiff;
    }

    shader->Use();
    shader->SetVec3("particleEmmiterPos", glm::vec3(startPosition, 0.0f));
}

void UIParticleGenerator::SetStartColor(glm::vec4 newColor)
{
    startColor = newColor;
    shader->Use();
    shader->SetVec4("startColor", startColor);
}

void UIParticleGenerator::SetEndColor(glm::vec4 newColor)
{
    endColor = newColor;
    shader->Use();
    shader->SetVec4("endColor", endColor);
}

void UIParticleGenerator::Update()
{
    glm::ivec2 wSize = Window::GetInstance()->GetContentSize();
    Particle* p;
    float dt = Time::GetDeltaTime();
    glm::vec2 randOffset(0.0f);
    float randAngle = 0.0f;
    float randRadius = 0.0f;

    for (unsigned int i = 0; i < amount; ++i)
    {
        p = particles[i];
        p->Life += dt; // reduce life
        particlesPos[i].x += p->Velocity.x * dt;
        particlesPos[i].y = startPosition.y + p->Velocity.y * p->Life + 0.5f * g * p->Life * p->Life;
        if (p->Life > maxLife)
        {
            if (generationRadius > 0.01) {
                randOffset.x = Random::Range<float>(-generationRadius, generationRadius) / (wSize.x * 0.5f);
                particlesPos[i] = startPosition + randOffset;
            }
            else {
                particlesPos[i] = startPosition;
            }

            p->Life = 0.0f;
        }
    }
}

// render all particles
void UIParticleGenerator::Draw()
{
    glDisable(GL_DEPTH_TEST);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticleSystemsController::Instance()->ParcticleSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, amount * sizeof(glm::vec2), particlesPos.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    //glActiveTexture(GL_TEXTURE0 + textureBinded);
    //glBindTexture(GL_TEXTURE_2D, _textures[i]);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    
    glEnable(GL_DEPTH_TEST);
}