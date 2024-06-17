#pragma once

#include <core/GameObject.h>
#include <core/Random.h>
#include <graphic/Shader.h>
#include <core/Time.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

class CityLightsComponent : public Component {
public:
	Transform* transform;
	Shader* cityLightsShader = nullptr;
	glm::vec3 lightColor = glm::vec3(1.0f, 0.0f, 0.0f);
	float maxVel = 0.4;
	glm::vec3 colorChangeVel = glm::vec3(0.0f, -maxVel, 0.0f);
	float lightAlpha = 0.8;

	float angle = 0.0f;
	float velocity = 60.0f;

	virtual void Initialize() override {
		cityLightsShader = ShaderManager::GetShaderProgram("origin/CityLights");
		cityLightsShader->Use();
		cityLightsShader->SetVec3("light_color", lightColor);
		cityLightsShader->SetFloat("light_alpha", lightAlpha);
		transform = GetTransform();

		float angle = Random::Range(0.0f, 360.0f);
		float v = Random::Range(-1.0f, 1.0f);
		if (v < 0.0f) {
			velocity *= -1.0f;
		}
	}

	virtual void Update() override {
		lightColor += colorChangeVel * Time::GetDeltaTime();

		if (lightColor.r > 1.0f) {
			lightColor.r = 1.0f;
			colorChangeVel.r = 0.0f;
			colorChangeVel.b = -maxVel;
		}
		else if (lightColor.r < 0.0f) {
			lightColor.r = 0.0f;
			colorChangeVel.b = maxVel;
		}

		if (lightColor.g > 1.0f) {
			lightColor.g = 1.0f;
			colorChangeVel.g = 0.0f;
			colorChangeVel.r = -maxVel;
		}
		else if (lightColor.g < 0.0f) {
			lightColor.g = 0.0f;
			colorChangeVel.r = maxVel;
		}

		if (lightColor.b > 1.0f) {
			lightColor.b = 1.0f;
			colorChangeVel.b = 0.0f;
			colorChangeVel.g = -maxVel;
		}
		else if (lightColor.b < 0.0f) {
			lightColor.b = 0.0f;
			colorChangeVel.g = maxVel;
		}

		cityLightsShader->Use();
		cityLightsShader->SetVec3("light_color", lightColor);

		angle += velocity * Time::GetDeltaTime();
		if (angle > 360.0f) {
			angle = 0.0f;
		}
		else if (angle < 0.0f) {
			angle = 360.0f;
		}
		transform->SetLocalRotation(glm::vec3(-15.0f * glm::sin(Time::GetTime()), angle, 15.0f * glm::sin(Time::GetTime())));
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "CityLightsComponent";
		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!Component::Deserialize(node))
			return false;

		return true;
	}

#if _DEBUG
	virtual void DrawEditor() override {
		string id = string(std::to_string(this->GetId()));
		string name = string("CityLightsComponent##Component").append(id);
		if (ImGui::CollapsingHeader(name.c_str())) {

			if (Component::DrawInheritedFields()) return;

			ImGui::ColorEdit3(string("lightColor##").append(id).c_str(), glm::value_ptr(lightColor));
			ImGui::DragFloat(string("maxVel##").append(id).c_str(), &maxVel, 0.01f, 10.0f);
			ImGui::DragFloat(string("lightAlpha##").append(id).c_str(), &lightAlpha);
			ImGui::DragFloat(string("velocity##").append(id).c_str(), &velocity);

			cityLightsShader->Use();
			cityLightsShader->SetFloat("light_alpha", lightAlpha);
		}
	}
#endif
};