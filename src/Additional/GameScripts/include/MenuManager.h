#pragma once

#include <core/Component.h>

#include <string>

#include <UI/Button.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;

class MenuManager : public Component {
private:

	Twin2Engine::UI::Button* _start = nullptr;
	Twin2Engine::UI::Button* _exit = nullptr;

	int _startButtonEvent = -1;
	int _exitButtonEvent = -1;

	void Start() {
		SceneManager::LoadScene("Game");
	}

	void Exit() {
		Window::GetInstance()->Close();
	}

	void InitializeButtons() {
		if (_startButtonEvent == -1 && _start != nullptr) _startButtonEvent = _start->GetOnClickEvent() += [&]() -> void { Start(); };
		if (_exitButtonEvent == -1 && _exit != nullptr) _exitButtonEvent = _exit->GetOnClickEvent() += [&]() -> void { Exit(); };
	}

public:
	virtual void Initialize() override {
		InitializeButtons();
	}

	virtual void Update() override {}

	virtual void OnDestroy() override {
		if (_start != nullptr && _startButtonEvent != -1) {
			_start->GetOnClickEvent() -= _startButtonEvent;
			_startButtonEvent = -1;
		}

		if (_exit != nullptr && _exitButtonEvent != -1) {
			_exit->GetOnClickEvent() -= _exitButtonEvent;
			_exitButtonEvent = -1;
		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "MenuManager";
		node["startButtonId"] = _start != nullptr ? _start->GetId() : 0;
		node["exitButtonId"] = _exit != nullptr ? _exit->GetId() : 0;

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["startButtonId"] || !node["exitButtonId"] || !Component::Deserialize(node))
			return false;

		_start = (Button*)SceneManager::GetComponentWithId(node["startButtonId"].as<size_t>());
		_exit = (Button*)SceneManager::GetComponentWithId(node["exitButtonId"].as<size_t>());

		return true;
	}

#if _DEBUG
	virtual void DrawEditor() override {
		std::string id = std::string(std::to_string(this->GetId()));
		std::string name = std::string("MenuManager##Component").append(id);
		if (ImGui::CollapsingHeader(name.c_str())) {

			if (Component::DrawInheritedFields()) return;

			unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Button>();
			size_t choosed_1 = _start == nullptr ? 0 : _start->GetId();

			if (ImGui::BeginCombo(string("StartButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_start != nullptr) {
							if (_startButtonEvent != -1) {
								_start->GetOnClickEvent() -= _startButtonEvent;
								_startButtonEvent = -1;
							}
						}

						_start = static_cast<Button*>(items[choosed_1]);

						if (_start != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _exit == nullptr ? 0 : _exit->GetId();

			if (ImGui::BeginCombo(string("ExitButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_exit != nullptr) {
							if (_exitButtonEvent != -1) {
								_exit->GetOnClickEvent() -= _exitButtonEvent;
								_exitButtonEvent = -1;
							}
						}

						_exit = static_cast<Button*>(items[choosed_1]);

						if (_exit != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			items.clear();
		}
	}
#endif
};