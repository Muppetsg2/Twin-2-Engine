#pragma once 

#include <core/GameObject.h>
#include <core/Component.h>

using namespace Twin2Engine::Core;

class Cloud : public Component {
	public:
		glm::vec3 startingScale = glm::vec3(1.0, 0.8, 1.2);
		glm::vec3 scaleVelocity = glm::vec3(0.1, 0.1, 0.1);

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
		virtual void DrawEditor() override;
#endif
};