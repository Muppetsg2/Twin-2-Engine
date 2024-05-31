#pragma once

#include <tools/STD140Struct.h>

namespace Twin2Engine::Graphic {
	struct Light {};

	struct PointLight : public Light {
		alignas(16) glm::vec3 position;      // Position of the point light in world space
		alignas(16) glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);         // Color of the point light
		float power = 1.0f;		  // Light source power
		float constant = 1.0f;     // Constant attenuation
		float linear = 0.0f;       // Linear attenuation
		float quadratic = 0.0f;    // Quadratic attenuation
	};

	struct SpotLight : public Light {
		alignas(16) glm::vec3 position;      // Position of the spot light in world space
		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);     // Direction of the spot light
		float power = 1.0f;		  // Light source power
		glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);         // Color of the spot light
		float innerCutOff = 0.0f; // Inner cutoff angle (in radians)
		float outerCutOff = 0.785f;  // Outer cutoff angle (in radians)
		float constant = 1.0f;     // Constant attenuation
		float linear = 0.0f;       // Linear attenuation
		float quadratic = 0.0f;    // Quadratic attenuation
	};

	struct DirectionalLight : public Light {
		glm::vec3 direction = glm::vec3(-0.7f, -0.7f, 0.0f);     // Direction of the spot light // (-0.32f, -0.95f, 0.0f)
		GLuint shadowMap = 0;
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);         // Color of the spot light
		GLuint shadowMapFBO = 0;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		float power = 1.0f;		  // Light source power
		GLuint shadowMapDynamic = 0;
		int padding1 = 0;
		int padding2 = 0;
	};
}