#pragma once

#include <core/Component.h>
#include <manager/SceneManager.h>

#include <string>

#include <UI/Button.h>

#include <UIScripts/PatronChoicePanelController.h>

class PauseManager : public Twin2Engine::Core::Component {
private:

	Twin2Engine::Core::GameObject* _pauseCanvas;

	Twin2Engine::UI::Button* _pause = nullptr;
	Twin2Engine::UI::Button* _resume = nullptr;
	Twin2Engine::UI::Button* _menu = nullptr;

	int _pauseButtonEvent = -1;
	int _resumeButtonEvent = -1;
	int _menuButtonEvent = -1;

	void Pause() {
		GameManager::instance->minigameActive = true;
		if (_pauseCanvas != nullptr) _pauseCanvas->SetActive(true);
		if (_pause != nullptr) _pause->SetInteractable(false);

		std::unordered_map<size_t, Twin2Engine::Core::Component*> items = SceneManager::GetComponentsOfType<PatronChoicePanelController>();

		PatronChoicePanelController* p = nullptr;
		for (auto& i : items) {
			p = dynamic_cast<PatronChoicePanelController*>(i.second);
			if (p != nullptr) if (!p->IsChoosed()) p->StopChoose();
		}

		items.clear();
	}

	void Resume() {
		GameManager::instance->minigameActive = false;
		if (_pauseCanvas != nullptr) _pauseCanvas->SetActive(false);
		if (_pause != nullptr) _pause->SetInteractable(true);

		std::unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<PatronChoicePanelController>();

		PatronChoicePanelController* p = nullptr;
		for (auto& i : items) {
			p = dynamic_cast<PatronChoicePanelController*>(i.second);
			if (p != nullptr) if (!p->IsChoosed()) p->StartChoose();
		}

		items.clear();
	}

	void Menu() {
		SceneManager::LoadScene("Menu");
	}

	void InitializeButtons() {
		if (_pauseButtonEvent == -1 && _pause != nullptr) _pauseButtonEvent = _pause->GetOnClickEvent() += [&]() -> void { Pause(); };
		if (_resumeButtonEvent == -1 && _resume != nullptr) _resumeButtonEvent = _resume->GetOnClickEvent() += [&]() -> void { Resume(); };
		if (_menuButtonEvent == -1 && _menu != nullptr) _menuButtonEvent = _menu->GetOnClickEvent() += [&]() -> void { Menu(); };
	}

public:
	virtual void Initialize() override {
		InitializeButtons();

		if (_pauseCanvas != nullptr) _pauseCanvas->SetActive(false);
	}

	virtual void Update() override {}

	virtual void OnDestroy() override {
		if (_pause != nullptr && _pauseButtonEvent != -1) {
			_pause->GetOnClickEvent() -= _pauseButtonEvent;
			_pauseButtonEvent = -1;
		}

		if (_resume != nullptr && _resumeButtonEvent != -1) {
			_resume->GetOnClickEvent() -= _resumeButtonEvent;
			_resumeButtonEvent = -1;
		}

		if (_menu != nullptr && _menuButtonEvent != -1) {
			_menu->GetOnClickEvent() -= _menuButtonEvent;
			_menuButtonEvent = -1;
		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "PauseManager";
		node["pauseButtonId"] = _pause != nullptr ? _pause->GetId() : 0;
		node["resumeButtonId"] = _resume != nullptr ? _resume->GetId() : 0;
		node["menuButtonId"] = _menu != nullptr ? _menu->GetId() : 0;

		node["pauseCanvas"] = _pauseCanvas != nullptr ? _pauseCanvas->Id() : 0;

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["pauseButtonId"] || !node["resumeButtonId"] || !node["menuButtonId"] ||
			!node["pauseCanvas"] || !Component::Deserialize(node))
			return false;

		size_t id = node["pauseButtonId"].as<size_t>();
		_pause = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["resumeButtonId"].as<size_t>();
		_resume = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["menuButtonId"].as<size_t>();
		_menu = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["pauseCanvas"].as<size_t>();
		_pauseCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

		return true;
	}

#if _DEBUG
	virtual void DrawEditor() override {
		std::string id = std::string(std::to_string(this->GetId()));
		std::string name = std::string("PauseManager##Component").append(id);
		if (ImGui::CollapsingHeader(name.c_str())) {

			if (Component::DrawInheritedFields()) return;

			unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Button>();
			size_t choosed_1 = _pause == nullptr ? 0 : _pause->GetId();

			if (ImGui::BeginCombo(string("PauseButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_pause != nullptr) {
							if (_pauseButtonEvent != -1) {
								_pause->GetOnClickEvent() -= _pauseButtonEvent;
								_pauseButtonEvent = -1;
							}
						}

						_pause = static_cast<Button*>(items[choosed_1]);

						if (_pause != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _resume == nullptr ? 0 : _resume->GetId();

			if (ImGui::BeginCombo(string("ResumeButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_resume != nullptr) {
							if (_resumeButtonEvent != -1) {
								_resume->GetOnClickEvent() -= _resumeButtonEvent;
								_resumeButtonEvent = -1;
							}
						}

						_resume = static_cast<Button*>(items[choosed_1]);

						if (_resume != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _menu == nullptr ? 0 : _menu->GetId();

			if (ImGui::BeginCombo(string("MenuButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_menu != nullptr) {
							if (_menuButtonEvent != -1) {
								_menu->GetOnClickEvent() -= _menuButtonEvent;
								_menuButtonEvent = -1;
							}
						}

						_menu = static_cast<Button*>(items[choosed_1]);

						if (_menu != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			items.clear();

			std::vector<GameObject*> objs = SceneManager::GetAllGameObjects();
			objs.erase(objs.begin());
			objs.insert(objs.begin(), nullptr);
			choosed_1 = std::find(objs.begin(), objs.end(), _pauseCanvas) - objs.begin();

			if (ImGui::BeginCombo(string("PauseCanvas##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

				bool clicked = false;
				for (size_t i = 0; i < objs.size(); ++i) {

					if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

						if (clicked) continue;

						choosed_1 = i;
						clicked = true;
					}
				}

				if (clicked) {
					_pauseCanvas = objs[choosed_1];
				}

				ImGui::EndCombo();
			}

			objs.clear();
		}
	}
#endif
};