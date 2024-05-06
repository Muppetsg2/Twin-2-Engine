#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <tools/STD140Struct.h>

namespace Twin2Engine::Graphic {
	struct Light {

	};

	// POINT LIGHT
	struct PointLight : public Light {
		glm::vec3 position;      // Position of the point light in world space
		glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);         // Color of the point light
		float power = 1.0f;		  // Light source power
		float constant = 1.0f;     // Constant attenuation
		float linear = 0.0f;       // Linear attenuation
		float quadratic = 0.0f;    // Quadratic attenuation
	};
	static const Tools::STD140Offsets PointLightOffsets{
		Tools::STD140Variable<glm::vec3>("position"),
		Tools::STD140Variable<glm::vec3>("color"),
		Tools::STD140Variable<float>("power"),
		Tools::STD140Variable<float>("constant"),
		Tools::STD140Variable<float>("linear"),
		Tools::STD140Variable<float>("quadratic")
	};
	static Tools::STD140Struct MakeStruct(const PointLight& pointLight) {
		Tools::STD140Struct value(PointLightOffsets);
		value.Set("position", pointLight.position);
		value.Set("color", pointLight.color);
		value.Set("power", pointLight.power);
		value.Set("constant", pointLight.constant);
		value.Set("linear", pointLight.linear);
		value.Set("quadratic", pointLight.quadratic);
		return value;
	}

	// SPOT LIGHT
	struct SpotLight : public Light {
		glm::vec3 position;      // Position of the spot light in world space
		glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);         // Color of the spot light
		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);     // Direction of the spot light
		float power = 1.0f;		  // Light source power
		float innerCutOff = 0.0f; // Inner cutoff angle (in radians)
		float outerCutOff = 0.785f;  // Outer cutoff angle (in radians)
		float constant = 1.0f;     // Constant attenuation
		float linear = 0.0f;       // Linear attenuation
		float quadratic = 0.0f;    // Quadratic attenuation
	};
	static const Tools::STD140Offsets SpotLightOffsets{
		Tools::STD140Variable<glm::vec3>("position"),
		Tools::STD140Variable<glm::vec3>("color"),
		Tools::STD140Variable<glm::vec3>("direction"),
		Tools::STD140Variable<float>("power"),
		Tools::STD140Variable<float>("innerCutOff"),
		Tools::STD140Variable<float>("outerCutOff"),
		Tools::STD140Variable<float>("constant"),
		Tools::STD140Variable<float>("linear"),
		Tools::STD140Variable<float>("quadratic")
	};
	static Tools::STD140Struct MakeStruct(const SpotLight& spotLight) {
		Tools::STD140Struct value(SpotLightOffsets);
		value.Set("position", spotLight.position);
		value.Set("color", spotLight.color);
		value.Set("direction", spotLight.direction);
		value.Set("power", spotLight.power);
		value.Set("innerCutOff", spotLight.innerCutOff);
		value.Set("outerCutOff", spotLight.outerCutOff);
		value.Set("constant", spotLight.constant);
		value.Set("linear", spotLight.linear);
		value.Set("quadratic", spotLight.quadratic);
		return value;
	}

	// DIRECTIONAL LIGHT
	struct DirectionalLight : public Light {
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);         // Color of the spot light
		glm::vec3 direction = glm::vec3(-0.7f, -0.7f, 0.0f);     // Direction of the spot light // (-0.32f, -0.95f, 0.0f)
		float power = 1.0f;		  // Light source power
		GLuint shadowMapFBO = 0;
		GLuint shadowMap = 0;
	};
	static const Tools::STD140Offsets DirectionalLightOffsets{
		Tools::STD140Variable<glm::mat4>("lightSpaceMatrix"),
		Tools::STD140Variable<glm::vec3>("color"),
		Tools::STD140Variable<glm::vec3>("direction"),
		Tools::STD140Variable<float>("power"),
		Tools::STD140Variable<GLuint>("shadowMapFBO"),
		Tools::STD140Variable<GLuint>("shadowMap"),
	};
	static Tools::STD140Struct MakeStruct(const DirectionalLight& dirLight) {
		Tools::STD140Struct value(DirectionalLightOffsets);
		value.Set("lightSpaceMatrix", dirLight.lightSpaceMatrix);
		value.Set("color", dirLight.color);
		value.Set("direction", dirLight.direction);
		value.Set("power", dirLight.power);
		value.Set("shadowMapFBO", dirLight.shadowMapFBO);
		value.Set("shadowMap", dirLight.shadowMap);
		return value;
	}
}

#endif // !_LIGHT_H_
