#ifndef _PARTICLESYSTEMSCONTROLLER_H_
#define _PARTICLESYSTEMSCONTROLLER_H_

#include <ParticleGenerator.h>
#include <unordered_set>

class ParticleSystemsController {
private:
	static ParticleSystemsController* instance;

	ParticleSystemsController() {}
public:
	~ParticleSystemsController() {
		ParticleGenerator* pg;

		while (particlesGenerators.size() > 0) {
			pg = *particlesGenerators.begin();
			particlesGenerators.erase(pg);
			delete pg;
		}
	}

	std::unordered_set<ParticleGenerator*> particlesGenerators;
	static ParticleSystemsController* Instance();
	static void DeleteInstance();

	void Update();
	void Render();
};

#endif // !_PARTICLESYSTEMSCONTROLLER_H_
