#pragma once

#include <core/GameObject.h>
#include <core/Time.h>

using namespace Twin2Engine::Core;

class StarComponent : public Component {
public:
	Transform* transform = nullptr;
	bool animatePosition = true;
	float startY = 0.45f;
	float posChangeScale = 0.05f;
	float timeScale = 0.6f;
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
			transform->SetGlobalRotation(glm::vec3(0.0f, (rotationMaxAngle * glm::sin(time * rotationVel)), 0.0f));
		}

		if (animateScale) {

		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "StarComponent";
		node["animatePosition"] = animatePosition;
		node["startY"] = startY;
		node["posChangeScale"] = posChangeScale;
		node["timeScale"] = timeScale;
		node["animateRotation"] = animateRotation;
		node["rotationVel"] = rotationVel;
		node["rotationMaxAngle"] = rotationMaxAngle;
		node["animateScale"] = animateScale;
		node["scale"] = scale;
		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["animatePosition"] || !node["startY"] || !node["posChangeScale"] ||
			!node["timeScale"] || !node["animateRotation"] || !node["rotationVel"] ||
			!node["rotationMaxAngle"] || !node["animateScale"] || !node["scale"] ||
			!Component::Deserialize(node))
			return false;

		animatePosition = node["animatePosition"].as<bool>();
		startY = node["startY"].as<float>();
		posChangeScale = node["posChangeScale"].as<float>();
		timeScale = node["timeScale"].as<float>();
		animateRotation = node["animateRotation"].as<bool>();
		rotationVel = node["rotationVel"].as<float>();
		rotationMaxAngle = node["rotationMaxAngle"].as<float>();
		animateScale = node["animateScale"].as<bool>();
		scale = node["scale"].as<float>();

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