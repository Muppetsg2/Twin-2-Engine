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
	Twin2Engine::UI::Image* _daltCheckImage = nullptr;

	Twin2Engine::UI::Button* _fullscreen = nullptr;
	Twin2Engine::UI::Image* _fullCheckImage = nullptr;

	bool _daltonistValue = false;
	bool _fullscreenValue = false;

	int _daltonistButtonEvent = -1;
	int _fullscreenButtonEvent = -1;

	void Daltonist() {
		_daltonistValue = !_daltonistValue;

		_daltCheckImage->GetGameObject()->SetActive(_daltonistValue);

		PlayerPrefs::SetValue("textureCoding", _daltonistValue);
	}

	void Fullscreen() {
		_fullscreenValue = !_fullscreenValue;

		_fullCheckImage->GetGameObject()->SetActive(_fullscreenValue);

		PlayerPrefs::SetValue("fullscreen", _fullscreenValue);

		if (_fullscreenValue && Window::GetInstance()->IsWindowed()) Window::GetInstance()->SetFullscreen(glfwGetPrimaryMonitor());
		else if (!_fullscreenValue && Window::GetInstance()->IsFullscreen()) Window::GetInstance()->SetWindowed({ 0, 30 }, { Window::GetInstance()->GetWindowSize().x, Window::GetInstance()->GetWindowSize().y - 50 });
	}

	void InitializeButtons() {
		if (_daltonistButtonEvent == -1 && _daltonist != nullptr) _daltonistButtonEvent = _daltonist->GetOnClickEvent() += [&]() -> void { Daltonist(); };
		if (_fullscreenButtonEvent == -1 && _fullscreen != nullptr) _fullscreenButtonEvent = _fullscreen->GetOnClickEvent() += [&]() -> void { Fullscreen(); };
	}

public:
	virtual void Initialize() override {
		InitializeButtons();

		_daltonistValue = PlayerPrefs::GetValue<bool>("textureCoding", false);
		_fullscreenValue = PlayerPrefs::GetValue<bool>("fullscreen", false);

		if (_daltCheckImage != nullptr) _daltCheckImage->GetGameObject()->SetActive(_daltonistValue);
		if (_fullCheckImage != nullptr) _fullCheckImage->GetGameObject()->SetActive(_fullscreenValue);

		if (_fullscreenValue && Window::GetInstance()->IsWindowed()) Window::GetInstance()->SetFullscreen(glfwGetPrimaryMonitor());
		else if (!_fullscreenValue && Window::GetInstance()->IsFullscreen()) Window::GetInstance()->SetWindowed({ 0, 30 }, { Window::GetInstance()->GetWindowSize().x, Window::GetInstance()->GetWindowSize().y - 50 });
	}

	virtual void OnDestroy() override {
		if (_daltonist != nullptr && _daltonistButtonEvent != -1) {
			_daltonist->GetOnClickEvent() -= _daltonistButtonEvent;
			_daltonistButtonEvent = -1;
		}

		if (_fullscreen != nullptr && _fullscreenButtonEvent != -1) {
			_fullscreen->GetOnClickEvent() -= _fullscreenButtonEvent;
			_fullscreenButtonEvent = -1;
		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "SettingsManager";
		node["daltonistButtonId"] = _daltonist != nullptr ? _daltonist->GetId() : 0;
		node["daltCheckImageId"] = _daltCheckImage != nullptr ? _daltCheckImage->GetId() : 0;
		node["fullscreenButtonId"] = _fullscreen != nullptr ? _fullscreen->GetId() : 0;
		node["fullCheckImageId"] = _fullCheckImage != nullptr ? _fullCheckImage->GetId() : 0;

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["daltonistButtonId"] || !node["daltCheckImageId"] ||
			!node["fullscreenButtonId"] || !node["fullCheckImageId"] ||
			!Component::Deserialize(node))
			return false;

		size_t id = node["daltonistButtonId"].as<size_t>();
		_daltonist = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["daltCheckImageId"].as<size_t>();
		_daltCheckImage = id != 0 ? (Image*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["fullscreenButtonId"].as<size_t>();
		_fullscreen = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["fullCheckImageId"].as<size_t>();
		_fullCheckImage = id != 0 ? (Image*)SceneManager::GetComponentWithId(id) : nullptr;

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

			choosed_1 = _fullscreen == nullptr ? 0 : _fullscreen->GetId();

			if (ImGui::BeginCombo(string("FullscreenButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_fullscreen != nullptr) {
							if (_fullscreenButtonEvent != -1) {
								_fullscreen->GetOnClickEvent() -= _fullscreenButtonEvent;
								_fullscreenButtonEvent = -1;
							}
						}

						_fullscreen = static_cast<Button*>(items[choosed_1]);

						if (_fullscreen != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			items.clear();

			items = SceneManager::GetComponentsOfType<Image>();

			choosed_1 = _daltCheckImage == nullptr ? 0 : _daltCheckImage->GetId();

			if (ImGui::BeginCombo(string("DaltonistCheckImage##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
						_daltCheckImage = static_cast<Image*>(items[choosed_1]);
						if (_daltCheckImage != nullptr) _daltCheckImage->GetGameObject()->SetActive(_daltonistValue);
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _fullCheckImage == nullptr ? 0 : _fullCheckImage->GetId();

			if (ImGui::BeginCombo(string("FullscreenCheckImage##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
						_fullCheckImage = static_cast<Image*>(items[choosed_1]);
						if (_fullCheckImage != nullptr) _fullCheckImage->GetGameObject()->SetActive(_fullscreenValue);
					}
				}

				ImGui::EndCombo();
			}
		}
	}
#endif
};