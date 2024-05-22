#pragma once 


using namespace Twin2Engine::Core;

class Cloud : public Component {
	public:
		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
		float velocity = 0.2;
		glm::vec3 startPosition = glm::vec3(-7.0f, 8.0f, -5.0f);
		float maxDistance = 10.0f;

		virtual void Initialize() override;
		virtual void Update() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		virtual void DrawEditor() override;
};