#include <ParticleSystemsController.h>

ParticleSystemsController* ParticleSystemsController::instance = nullptr;

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
}

void ParticleSystemsController::Render()
{
	for (auto& pg : particlesGenerators) {
		if (pg->active) {
			pg->Draw();
		}
	}
}
