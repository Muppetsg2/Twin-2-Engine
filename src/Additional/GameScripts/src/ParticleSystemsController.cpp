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
	int i = 0;
	for (auto& pg : particlesGenerators) {
		//SPDLOG_INFO("{}. {} \t StartPos:\t{}\t{}", i++, (unsigned long long)pg, pg->startPosition.x, pg->startPosition.z);
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
