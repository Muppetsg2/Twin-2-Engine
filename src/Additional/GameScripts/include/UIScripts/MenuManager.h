#pragma once

#include <core/Component.h>

#include <string>

#include <UI/Button.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;

class MenuManager : public Component {
private:

	Twin2Engine::Core::GameObject* _menuCanvas;
	Twin2Engine::Core::GameObject* _creditsCanvas;

	Twin2Engine::UI::Button* _start = nullptr;
	Twin2Engine::UI::Button* _credits = nullptr;
	Twin2Engine::UI::Button* _creditsBack = nullptr;
	Twin2Engine::UI::Button* _exit = nullptr;

	int _startButtonEvent = -1;
	int _creditsButtonEvent = -1;
	int _creditsBackButtonEvent = -1;
	int _exitButtonEvent = -1;

	void Start() {
		SceneManager::LoadScene("Game");
	}

	void Credits() {
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(false);
		if (_creditsCanvas != nullptr) _creditsCanvas->SetActive(true);
	}

	void CreditsBack() {
		if (_creditsCanvas != nullptr) _creditsCanvas->SetActive(false);
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(true);
	}

	void Exit() {
		Window::GetInstance()->Close();
	}

	void InitializeButtons() {
		if (_startButtonEvent == -1 && _start != nullptr) _startButtonEvent = _start->GetOnClickEvent() += [&]() -> void { Start(); };
		if (_creditsButtonEvent == -1 && _credits != nullptr) _creditsButtonEvent = _credits->GetOnClickEvent() += [&]() -> void { Credits(); };
		if (_creditsBackButtonEvent == -1 && _creditsBack != nullptr) _creditsBackButtonEvent = _creditsBack->GetOnClickEvent() += [&]() -> void { CreditsBack(); };
		if (_exitButtonEvent == -1 && _exit != nullptr) _exitButtonEvent = _exit->GetOnClickEvent() += [&]() -> void { Exit(); };
	}

public:
	virtual void Initialize() override {
		InitializeButtons();

		if (_menuCanvas != nullptr) _menuCanvas->SetActive(true);
		if (_creditsCanvas != nullptr) _creditsCanvas->SetActive(false);
	}

	virtual void Update() override {}

	virtual void OnDestroy() override {
		if (_start != nullptr && _startButtonEvent != -1) {
			_start->GetOnClickEvent() -= _startButtonEvent;
			_startButtonEvent = -1;
		}

		if (_credits != nullptr && _creditsButtonEvent != -1) {
			_credits->GetOnClickEvent() -= _creditsButtonEvent;
			_creditsButtonEvent = -1;
		}

		if (_creditsBack != nullptr && _creditsBackButtonEvent != -1) {
			_creditsBack->GetOnClickEvent() -= _creditsBackButtonEvent;
			_creditsBackButtonEvent = -1;
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
		node["creditsButtonId"] = _credits != nullptr ? _credits->GetId() : 0;
		node["creditsBackButtonId"] = _creditsBack != nullptr ? _creditsBack->GetId() : 0;
		node["exitButtonId"] = _exit != nullptr ? _exit->GetId() : 0;

		node["menuCanvas"] = _menuCanvas != nullptr ? _menuCanvas->Id() : 0;
		node["creditsCanvas"] = _creditsCanvas != nullptr ? _creditsCanvas->Id() : 0;

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["startButtonId"] || !node["creditsButtonId"] || !node["creditsBackButtonId"] || !node["exitButtonId"] || 
			!node["menuCanvas"] || !node["creditsCanvas"] || !Component::Deserialize(node))
			return false;

		size_t id = node["startButtonId"].as<size_t>();
		_start = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["creditsButtonId"].as<size_t>();
		_credits = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["creditsBackButtonId"].as<size_t>();
		_creditsBack = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["exitButtonId"].as<size_t>();
		_exit = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["menuCanvas"].as<size_t>();
		_menuCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

		id = node["creditsCanvas"].as<size_t>();
		_creditsCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

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

			choosed_1 = _credits == nullptr ? 0 : _credits->GetId();

			if (ImGui::BeginCombo(string("CreditsButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_credits != nullptr) {
							if (_creditsButtonEvent != -1) {
								_credits->GetOnClickEvent() -= _creditsButtonEvent;
								_creditsButtonEvent = -1;
							}
						}

						_credits = static_cast<Button*>(items[choosed_1]);

						if (_credits != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _creditsBack == nullptr ? 0 : _creditsBack->GetId();

			if (ImGui::BeginCombo(string("CreditsBackButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_creditsBack != nullptr) {
							if (_creditsBackButtonEvent != -1) {
								_creditsBack->GetOnClickEvent() -= _creditsBackButtonEvent;
								_creditsBackButtonEvent = -1;
							}
						}

						_creditsBack = static_cast<Button*>(items[choosed_1]);

						if (_creditsBack != nullptr) {
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

			std::vector<GameObject*> objs = SceneManager::GetAllGameObjects();
			objs.erase(objs.begin());
			objs.insert(objs.begin(), nullptr);
			choosed_1 = std::find(objs.begin(), objs.end(), _menuCanvas) - objs.begin();

			if (ImGui::BeginCombo(string("MenuCanvas##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

				bool clicked = false;
				for (size_t i = 0; i < objs.size(); ++i) {

					if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

						if (clicked) continue;

						choosed_1 = i;
						clicked = true;
					}
				}

				if (clicked) {
					_menuCanvas = objs[choosed_1];
				}

				ImGui::EndCombo();
			}

			choosed_1 = std::find(objs.begin(), objs.end(), _creditsCanvas) - objs.begin();

			if (ImGui::BeginCombo(string("CreditsCanvas##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

				bool clicked = false;
				for (size_t i = 0; i < objs.size(); ++i) {

					if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

						if (clicked) continue;

						choosed_1 = i;
						clicked = true;
					}
				}

				if (clicked) {
					_creditsCanvas = objs[choosed_1];
				}

				ImGui::EndCombo();
			}

			objs.clear();
		}
	}
#endif
};