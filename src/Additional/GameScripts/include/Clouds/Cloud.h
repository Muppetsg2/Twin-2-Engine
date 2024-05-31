#pragma once 

#include <core/GameObject.h>
#include <core/Component.h>

using namespace Twin2Engine::Core;

class Cloud : public Component {
	public:
		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
		float velocity = 0.2;
		glm::vec3 startPosition = glm::vec3(-7.0f, 8.0f, -5.0f);
		float maxDistance = 10.0f;
		float travelledDistance = 0.0f;

		glm::vec3 startingScale = glm::vec3(1.0, 0.8, 1.2);
		glm::vec3 scaleVelocity = glm::vec3(0.1, 0.1, 0.1);

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		virtual void DrawEditor() override;
};