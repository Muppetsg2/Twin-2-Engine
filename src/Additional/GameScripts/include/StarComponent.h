#pragma once

#include <core/GameObject.h>
#include <core/Time.h>

using namespace Twin2Engine::Core;

class StarComponent : public Component {
public:
	Transform* transform = nullptr;
	bool animatePosition = true;
	float startY = 0.45;
	float posChangeScale = 0.05;
	float timeScale = 0.6;
	bool animateRotation = true;
	float rotationVel = 0.3f;
	float rotationMaxAngle = 30.0f;
	bool animateScale = true;

	float time = 0.0f;

	float scale = 8.0f;

	virtual void Initialize() override {
		transform = GetTransform();
	}

	virtual void Update() override {
		time += Time::GetDeltaTime();
		if (animatePosition) {
			transform->SetLocalPosition(glm::vec3(0.0f, scale *(startY + posChangeScale * glm::cos(time * timeScale)), 0.0f));
		}

		if (animateRotation) {
			transform->SetLocalRotation(glm::vec3(0.0f, (rotationMaxAngle * glm::sin(time * rotationVel)), 0.0f));
		}

		if (animateScale) {

		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "StarComponent";
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
		string name = string("StarComponent##Component").append(id);
		if (ImGui::CollapsingHeader(name.c_str())) {

			if (Component::DrawInheritedFields()) return;

			ImGui::DragFloat(string("startY##").append(id).c_str(), &startY);
			ImGui::DragFloat(string("posChangeScale##").append(id).c_str(), &posChangeScale);
			ImGui::DragFloat(string("timeScale##").append(id).c_str(), &timeScale);
			ImGui::DragFloat(string("rotationVel##").append(id).c_str(), &rotationVel);
			ImGui::DragFloat(string("rotationMaxAngle##").append(id).c_str(), &rotationMaxAngle);

			if (ImGui::Button("Reset Time")) {
				time = 0.0f;
			}
		}
	}
#endif
};