#include <ParticleSystemsController.h>

ParticleSystemsController* ParticleSystemsController::instance = nullptr;
const int ParticleSystemsController::MAX_NUMBER_OF_PARTICLE = 10;

ParticleSystemsController* ParticleSystemsController::Instance()
{
	if (instance == nullptr) {
		instance = new ParticleSystemsController();
	}

	return instance;
}

void ParticleSystemsController::DeleteInstance()
{
	if (instance != nullptr) {
		delete instance;
	}
}

void ParticleSystemsController::Update()
{
	for (auto& pg : particlesGenerators) {
		if (pg->active) {
			pg->Update();
		}
	}

	for (auto& pg : UIParticlesGeneratorsFront) {
		if (pg->active) {
			pg->Update();
		}
	}

	for (auto& pg : UIParticlesGeneratorsBack) {
		if (pg->active) {
			pg->Update();
		}
	}
}

void ParticleSystemsController::Render()
{
	for (auto& pg : particlesGenerators) {
		if (pg->active) {
			pg->Draw();
		}
	}
}

void ParticleSystemsController::RenderUIFront()
{
	for (auto& pg : UIParticlesGeneratorsFront) {
		if (pg->active) {
			pg->Draw();
		}
	}
}

void ParticleSystemsController::RenderUIBack()
{
	for (auto& pg : UIParticlesGeneratorsBack) {
		if (pg->active) {
			pg->Draw();
		}
	}
}
