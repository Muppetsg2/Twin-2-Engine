#pragma once

#include <ParticleGenerator.h>
#include <unordered_set>

class ParticleSystemsController {
	friend ParticleGenerator;
private:
	static ParticleSystemsController* instance;
	static const int MAX_NUMBER_OF_PARTICLE;

	unsigned int ParcticleSSBO;

	ParticleSystemsController() {
		glGenBuffers(1, &ParcticleSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParcticleSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_NUMBER_OF_PARTICLE * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ParcticleSSBO);

		//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, amount * sizeof(glm::vec4), particlesPos.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
public:
	~ParticleSystemsController() {
		ParticleGenerator* pg;
		glDeleteBuffers(1, &ParcticleSSBO);

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