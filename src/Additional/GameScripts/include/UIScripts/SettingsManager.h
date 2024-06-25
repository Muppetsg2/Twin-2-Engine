#pragma once

#include <core/Component.h>

#include <string>

#include <UI/Button.h>
#include <UI/Image.h>
#include <manager/SceneManager.h>
#include <core/PlayerPrefs.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::Manager;

class SettingsManager : public Component {
private:

	Twin2Engine::UI::Button* _daltonist = nullptr;
	Twin2Engine::UI::Image* _checkImage = nullptr;

	bool _daltonistValue = false;

	int _daltonistButtonEvent = -1;

	void Daltonist() {
		_daltonistValue = !_daltonistValue;

		_checkImage->GetGameObject()->SetActive(_daltonistValue);

		PlayerPrefs::SetValue("textureCoding", _daltonistValue);
	}

	void InitializeButtons() {
		if (_daltonistButtonEvent == -1 && _daltonist != nullptr) _daltonistButtonEvent = _daltonist->GetOnClickEvent() += [&]() -> void { Daltonist(); };
	}

public:
	virtual void Initialize() override {
		InitializeButtons();

		_daltonistValue = PlayerPrefs::GetValue<bool>("textureCoding", false);

		if (_checkImage != nullptr) _checkImage->GetGameObject()->SetActive(_daltonistValue);
	}

	virtual void OnDestroy() override {
		if (_daltonist != nullptr && _daltonistButtonEvent != -1) {
			_daltonist->GetOnClickEvent() -= _daltonistButtonEvent;
			_daltonistButtonEvent = -1;
		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "SettingsManager";
		node["daltonistButtonId"] = _daltonist != nullptr ? _daltonist->GetId() : 0;
		node["checkImageId"] = _checkImage != nullptr ? _checkImage->GetId() : 0;

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["daltonistButtonId"] || !node["checkImageId"] ||
			!Component::Deserialize(node))
			return false;

		size_t id = node["daltonistButtonId"].as<size_t>();
		_daltonist = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["checkImageId"].as<size_t>();
		_checkImage = id != 0 ? (Image*)SceneManager::GetComponentWithId(id) : nullptr;

		return true;
	}

#if _DEBUG
	virtual void DrawEditor() override {
		std::string id = std::string(std::to_string(this->GetId()));
		std::string name = std::string("SettingsManager##Component").append(id);
		if (ImGui::CollapsingHeader(name.c_str())) {

			if (Component::DrawInheritedFields()) return;

			unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Button>();
			size_t choosed_1 = _daltonist == nullptr ? 0 : _daltonist->GetId();

			if (ImGui::BeginCombo(string("DaltonistButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

				bool clicked = false;
				for (auto& item : items) {

					if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

						if (clicked) continue;

						choosed_1 = item.first;
						clicked = true;
					}
				}

				if (clicked) {
					if (choosed_1 != 0) {

						if (_daltonist != nullptr) {
							if (_daltonistButtonEvent != -1) {
								_daltonist->GetOnClickEvent() -= _daltonistButtonEvent;
								_daltonistButtonEvent = -1;
							}
						}

						_daltonist = static_cast<Button*>(items[choosed_1]);

						if (_daltonist != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			items.clear();

			items = SceneManager::GetComponentsOfType<Image>();

			choosed_1 = _checkImage == nullptr ? 0 : _checkImage->GetId();

			if (ImGui::BeginCombo(string("CheckImage##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

				bool clicked = false;
				for (auto& item : items) {

					if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

						if (clicked) continue;

						choosed_1 = item.first;
						clicked = true;
					}
				}

				if (clicked) {
					if (choosed_1 != 0) {
						_checkImage = static_cast<Image*>(items[choosed_1]);
						if (_checkImage != nullptr) _checkImage->GetGameObject()->SetActive(_daltonistValue);
					}
				}

				ImGui::EndCombo();
			}
		}
	}
#endif
};