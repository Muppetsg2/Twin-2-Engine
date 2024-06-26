#pragma once

#include <core/Component.h>
#include <core/Random.h>

#include <string>

#include <UI/Button.h>

extern Twin2Engine::Graphic::Window* window;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;

class MenuManager : public Component {
private:

	Twin2Engine::Core::GameObject* _menuCanvas;
	Twin2Engine::Core::GameObject* _creditsCanvas;
	Twin2Engine::Core::GameObject* _highscoreCanvas;
	Twin2Engine::Core::GameObject* _settingsCanvas;

	Twin2Engine::UI::Button* _start = nullptr;
	Twin2Engine::UI::Button* _credits = nullptr;
	Twin2Engine::UI::Button* _creditsBack = nullptr;
	Twin2Engine::UI::Button* _highscore = nullptr;
	Twin2Engine::UI::Button* _highscoreBack = nullptr;
	Twin2Engine::UI::Button* _settings = nullptr;
	Twin2Engine::UI::Button* _settingsBack = nullptr;
	Twin2Engine::UI::Button* _exit = nullptr;
	Twin2Engine::UI::Button* _tutorial = nullptr;

	Twin2Engine::UI::Text* _highscoreText = nullptr;

	Twin2Engine::Core::AudioComponent* _audio = nullptr;
	std::vector<size_t> _audios;

	int _startButtonEvent = -1;
	int _creditsButtonEvent = -1;
	int _creditsBackButtonEvent = -1;
	int _highscoreButtonEvent = -1;
	int _highscoreBackButtonEvent = -1;
	int _settingsButtonEvent = -1;
	int _settingsBackButtonEvent = -1;
	int _exitButtonEvent = -1;
	int _tutorialButtonEvent = -1;

	bool _loadScene = false;
	bool _loadTutorialScene = false;
	float _loadSceneCounter = 0.f;
	float _loadSceneTime = 2.f;

	void Start() {
		_loadScene = true;
		_loadSceneCounter = _loadSceneTime;
		//SceneManager::LoadScene("Game");

		if (_start != nullptr) _start->SetInteractable(false);
		if (_credits != nullptr) _credits->SetInteractable(false);
		if (_highscore != nullptr) _highscore->SetInteractable(false);
		if (_settings != nullptr) _settings->SetInteractable(false);
		if (_exit != nullptr) _exit->SetInteractable(false);
		if (_tutorial != nullptr) _tutorial->SetInteractable(false);
	}

	void StartTutorial() {
		_loadTutorialScene = true;
		_loadSceneCounter = _loadSceneTime;
		//SceneManager::LoadScene("Game");

		if (_start != nullptr) _start->SetInteractable(false);
		if (_credits != nullptr) _credits->SetInteractable(false);
		if (_highscore != nullptr) _highscore->SetInteractable(false);
		if (_settings != nullptr) _settings->SetInteractable(false);
		if (_exit != nullptr) _exit->SetInteractable(false);
		if (_tutorial != nullptr) _tutorial->SetInteractable(false);
	}

	void Credits() {
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(false);
		if (_creditsCanvas != nullptr) _creditsCanvas->SetActive(true);
	}

	void CreditsBack() {
		if (_creditsCanvas != nullptr) _creditsCanvas->SetActive(false);
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(true);
	}

	void Highscore() {
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(false);
		if (_highscoreCanvas != nullptr) _highscoreCanvas->SetActive(true);

		if (_highscoreText != nullptr) {
			int days = PlayerPrefs::GetValue<int>("HighestDay");
			int weeks = PlayerPrefs::GetValue<int>("HighestWeek");
			int months = PlayerPrefs::GetValue<int>("HighestMonth");
			int years = PlayerPrefs::GetValue<int>("HighestYear");
			_highscoreText->SetText(
				wstring(L"Years: ").append(to_wstring(years))
				.append(L"\nMonths: ").append(to_wstring(months))
				.append(L"\nWeeks: ").append(to_wstring(weeks))
				.append(L"\nDays: ").append(to_wstring(days)));
		}
	}

	void HighscoreBack() {
		if (_highscoreCanvas != nullptr) _highscoreCanvas->SetActive(false);
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(true);
	}

	void Settings() {
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(false);
		if (_settingsCanvas != nullptr) _settingsCanvas->SetActive(true);
	}

	void SettingsBack() {
		if (_settingsCanvas != nullptr) _settingsCanvas->SetActive(false);
		if (_menuCanvas != nullptr) _menuCanvas->SetActive(true);
	}

	void Exit() {
		Window::GetInstance()->Close();
	}

	void InitializeButtons() {
		if (_startButtonEvent == -1 && _start != nullptr) _startButtonEvent = _start->GetOnClickEvent() += [&]() -> void { Start(); };
		if (_creditsButtonEvent == -1 && _credits != nullptr) _creditsButtonEvent = _credits->GetOnClickEvent() += [&]() -> void { Credits(); };
		if (_creditsBackButtonEvent == -1 && _creditsBack != nullptr) _creditsBackButtonEvent = _creditsBack->GetOnClickEvent() += [&]() -> void { CreditsBack(); };
		if (_highscoreButtonEvent == -1 && _highscore != nullptr) _highscoreButtonEvent = _highscore->GetOnClickEvent() += [&]() -> void { Highscore(); };
		if (_highscoreBackButtonEvent == -1 && _highscoreBack != nullptr) _highscoreBackButtonEvent = _highscoreBack->GetOnClickEvent() += [&]() -> void { HighscoreBack(); };
		if (_settingsButtonEvent == -1 && _settings != nullptr) _settingsButtonEvent = _settings->GetOnClickEvent() += [&]() -> void { Settings(); };
		if (_settingsBackButtonEvent == -1 && _settingsBack != nullptr) _settingsBackButtonEvent = _settingsBack->GetOnClickEvent() += [&]() -> void { SettingsBack(); };
		if (_exitButtonEvent == -1 && _exit != nullptr) _exitButtonEvent = _exit->GetOnClickEvent() += [&]() -> void { Exit(); };
		if (_tutorialButtonEvent == -1 && _tutorial != nullptr) _tutorialButtonEvent = _tutorial->GetOnClickEvent() += [&]() -> void { StartTutorial(); };
	}

public:
	virtual void Initialize() override {
		InitializeButtons();

		if (_menuCanvas != nullptr) _menuCanvas->SetActive(true);
		if (_creditsCanvas != nullptr) _creditsCanvas->SetActive(false);
		if (_highscoreCanvas != nullptr) _highscoreCanvas->SetActive(false);
		if (_settingsCanvas != nullptr) _settingsCanvas->SetActive(false);

		if (_audio != nullptr && _audios.size() != 0) {
			_audio->Stop();
			_audio->SetAudio(_audios[Twin2Engine::Core::Random::Range(0ull, _audios.size() - 1)]);
			_audio->Play();
		}

		_loadScene = false;
		_loadTutorialScene = false;
	}

	virtual void Update() override {
		
#if !_DEBUG
		if (Input::IsKeyPressed(KEY::ESCAPE))
		{
			window->Close();
			return;
		}
#endif

		if (_loadScene) {
			_loadSceneCounter -= Time::GetDeltaTime();

			if (_audio != nullptr) _audio->SetVolume(_loadSceneCounter / _loadSceneTime);

			if (_loadSceneCounter <= 0.f) {
				SceneManager::LoadScene("Game");
			}
		}

		if (_loadTutorialScene) {
			_loadSceneCounter -= Time::GetDeltaTime();

			if (_audio != nullptr) _audio->SetVolume(_loadSceneCounter / _loadSceneTime);

			if (_loadSceneCounter <= 0.f) {
				SceneManager::LoadScene("GameWithTutorial");
			}
		}
	}

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

		if (_highscore != nullptr && _highscoreButtonEvent != -1) {
			_highscore->GetOnClickEvent() -= _highscoreButtonEvent;
			_highscoreButtonEvent = -1;
		}

		if (_highscoreBack != nullptr && _highscoreBackButtonEvent != -1) {
			_highscoreBack->GetOnClickEvent() -= _highscoreBackButtonEvent;
			_highscoreBackButtonEvent = -1;
		}

		if (_settings != nullptr && _settingsButtonEvent != -1) {
			_settings->GetOnClickEvent() -= _settingsButtonEvent;
			_settingsButtonEvent = -1;
		}

		if (_settingsBack != nullptr && _settingsBackButtonEvent != -1) {
			_settingsBack->GetOnClickEvent() -= _settingsBackButtonEvent;
			_settingsBackButtonEvent = -1;
		}

		if (_exit != nullptr && _exitButtonEvent != -1) {
			_exit->GetOnClickEvent() -= _exitButtonEvent;
			_exitButtonEvent = -1;
		}

		if (_tutorial != nullptr && _tutorialButtonEvent != -1) {
			_tutorial->GetOnClickEvent() -= _tutorialButtonEvent;
			_tutorialButtonEvent = -1;
		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "MenuManager";
		node["loadSceneTime"] = _loadSceneTime;
		node["startButtonId"] = _start != nullptr ? _start->GetId() : 0;
		node["creditsButtonId"] = _credits != nullptr ? _credits->GetId() : 0;
		node["creditsBackButtonId"] = _creditsBack != nullptr ? _creditsBack->GetId() : 0;
		node["highscoreButtonId"] = _highscore != nullptr ? _highscore->GetId() : 0;
		node["highscoreBackButtonId"] = _highscoreBack != nullptr ? _highscoreBack->GetId() : 0;
		node["settingsButtonId"] = _settings != nullptr ? _settings->GetId() : 0;
		node["settingsBackButtonId"] = _settingsBack != nullptr ? _settingsBack->GetId() : 0;
		node["exitButtonId"] = _exit != nullptr ? _exit->GetId() : 0;
		node["tutorialButtonId"] = _tutorial != nullptr ? _tutorial->GetId() : 0;

		node["menuCanvas"] = _menuCanvas != nullptr ? _menuCanvas->Id() : 0;
		node["creditsCanvas"] = _creditsCanvas != nullptr ? _creditsCanvas->Id() : 0;
		node["highscoreCanvas"] = _highscoreCanvas != nullptr ? _highscoreCanvas->Id() : 0;
		node["settingsCanvas"] = _settingsCanvas != nullptr ? _settingsCanvas->Id() : 0;

		node["highscoreText"] = _highscoreText != nullptr ? _highscoreText->GetId() : 0;

		node["audioId"] = _audio != nullptr ? _audio->GetId() : 0;
		node["audios"] = std::vector<size_t>();

		for (auto a : _audios) {
			node["audios"].push_back(Twin2Engine::Manager::SceneManager::GetAudioSaveIdx(a));
		}

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["loadSceneTime"] || !node["startButtonId"] || !node["creditsButtonId"] || !node["creditsBackButtonId"] || 
			!node["highscoreButtonId"] || !node["highscoreBackButtonId"] || !node["settingsButtonId"] || !node["settingsBackButtonId"] || 
			!node["exitButtonId"] || !node["tutorialButtonId"] || !node["menuCanvas"] || !node["creditsCanvas"] || !node["highscoreCanvas"] ||
			!node["settingsCanvas"] || !node["highscoreText"] ||
			!node["audioId"] || !node["audios"] || !Component::Deserialize(node))
			return false;

		_loadSceneTime = node["loadSceneTime"].as<float>();

		size_t id = node["startButtonId"].as<size_t>();
		_start = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["creditsButtonId"].as<size_t>();
		_credits = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["creditsBackButtonId"].as<size_t>();
		_creditsBack = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["highscoreButtonId"].as<size_t>();
		_highscore = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["highscoreBackButtonId"].as<size_t>();
		_highscoreBack = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["settingsButtonId"].as<size_t>();
		_settings = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["settingsBackButtonId"].as<size_t>();
		_settingsBack = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["exitButtonId"].as<size_t>();
		_exit = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["tutorialButtonId"].as<size_t>();
		_tutorial = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["menuCanvas"].as<size_t>();
		_menuCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

		id = node["creditsCanvas"].as<size_t>();
		_creditsCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

		id = node["highscoreCanvas"].as<size_t>();
		_highscoreCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

		id = node["settingsCanvas"].as<size_t>();
		_settingsCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

		id = node["highscoreText"].as<size_t>();
		_highscoreText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;

		id = node["audioId"].as<size_t>();
		_audio = id != 0 ? (AudioComponent*)SceneManager::GetComponentWithId(id) : nullptr;

		_audios = std::vector<size_t>();
		_audios.reserve(node["audios"].size());
		for (size_t i = 0; i < node["audios"].size(); ++i) {
			_audios.push_back(SceneManager::GetAudio(node["audios"][i].as<size_t>()));
		}

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

			choosed_1 = _highscore == nullptr ? 0 : _highscore->GetId();

			if (ImGui::BeginCombo(string("HighscoreButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_highscore != nullptr) {
							if (_highscoreButtonEvent != -1) {
								_highscore->GetOnClickEvent() -= _highscoreButtonEvent;
								_highscoreButtonEvent = -1;
							}
						}

						_highscore = static_cast<Button*>(items[choosed_1]);

						if (_highscore != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _highscoreBack == nullptr ? 0 : _highscoreBack->GetId();

			if (ImGui::BeginCombo(string("HighscoreBackButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_highscoreBack != nullptr) {
							if (_highscoreBackButtonEvent != -1) {
								_highscoreBack->GetOnClickEvent() -= _highscoreBackButtonEvent;
								_highscoreBackButtonEvent = -1;
							}
						}

						_highscoreBack = static_cast<Button*>(items[choosed_1]);

						if (_highscoreBack != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _settings == nullptr ? 0 : _settings->GetId();

			if (ImGui::BeginCombo(string("SettingsButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_settings != nullptr) {
							if (_settingsButtonEvent != -1) {
								_settings->GetOnClickEvent() -= _settingsButtonEvent;
								_settingsButtonEvent = -1;
							}
						}

						_settings = static_cast<Button*>(items[choosed_1]);

						if (_settings != nullptr) {
							InitializeButtons();
						}
					}
				}

				ImGui::EndCombo();
			}

			choosed_1 = _settingsBack == nullptr ? 0 : _settingsBack->GetId();

			if (ImGui::BeginCombo(string("SettingsBackButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

						if (_settingsBack != nullptr) {
							if (_settingsBackButtonEvent != -1) {
								_settingsBack->GetOnClickEvent() -= _settingsBackButtonEvent;
								_settingsBackButtonEvent = -1;
							}
						}

						_settingsBack = static_cast<Button*>(items[choosed_1]);

						if (_settingsBack != nullptr) {
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

			choosed_1 = std::find(objs.begin(), objs.end(), _highscoreCanvas) - objs.begin();

			if (ImGui::BeginCombo(string("HighscoreCanvas##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

				bool clicked = false;
				for (size_t i = 0; i < objs.size(); ++i) {

					if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

						if (clicked) continue;

						choosed_1 = i;
						clicked = true;
					}
				}

				if (clicked) {
					_highscoreCanvas = objs[choosed_1];
				}

				ImGui::EndCombo();
			}

			choosed_1 = std::find(objs.begin(), objs.end(), _settingsCanvas) - objs.begin();

			if (ImGui::BeginCombo(string("SettingsCanvas##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

				bool clicked = false;
				for (size_t i = 0; i < objs.size(); ++i) {

					if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

						if (clicked) continue;

						choosed_1 = i;
						clicked = true;
					}
				}

				if (clicked) {
					_settingsCanvas = objs[choosed_1];
				}

				ImGui::EndCombo();
			}

			objs.clear();

			items = SceneManager::GetComponentsOfType<Text>();
			choosed_1 = _highscoreText == nullptr ? 0 : _highscoreText->GetId();

			if (ImGui::BeginCombo(string("HighscoreText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
						_highscoreText = static_cast<Text*>(items[choosed_1]);
					}
				}

				ImGui::EndCombo();
			}


			items = SceneManager::GetComponentsOfType<AudioComponent>();
			choosed_1 = _audio == nullptr ? 0 : _audio->GetId();

			if (ImGui::BeginCombo(string("AudioPlayer##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
						_audio = static_cast<AudioComponent*>(items[choosed_1]);
					}
				}

				ImGui::EndCombo();
			}

			items.clear();

			ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			std::list<size_t> clicked = std::list<size_t>();
			clicked.clear();

			if (ImGui::TreeNodeEx(string("Audios##").append(id).c_str(), node_flag)) {
				size_t i = 0;
				for (auto a : _audios) {
					string n = AudioManager::GetAudioName(a).append("##").append(id).append(std::to_string(i));
					ImGui::Selectable(n.c_str(), false, NULL, ImVec2(ImGui::GetContentRegionAvail().x - 80, 0.f));

					ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
					if (ImGui::Button(string(ICON_FA_TRASH_CAN "##Remove").append(id).append(std::to_string(i)).c_str())) {
						clicked.push_back(i);
					}
					++i;
				}
				ImGui::TreePop();
			}

			if (clicked.size() > 0) {
				clicked.sort();

				for (int i = clicked.size() - 1; i > -1; --i)
				{
					_audios.erase(_audios.begin() + clicked.back());

					clicked.pop_back();
				}
			}

			clicked.clear();

			if (ImGui::Button(string("Add Audio##").append(id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
				ImGui::OpenPopup(string("Add Audio PopUp##Menu Manager").append(id).c_str(), ImGuiPopupFlags_NoReopen);
			}

			if (ImGui::BeginPopup(string("Add Audio PopUp##Menu Manager").append(id).c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {

				std::map<size_t, std::string> types = AudioManager::GetAllAudiosNames();

				std::vector<std::pair<size_t, std::string>> audioNames = std::vector<std::pair<size_t, std::string>>(types.begin(), types.end());

				types.clear();

				std::sort(audioNames.begin(), audioNames.end(), [&](std::pair<size_t, std::string> const& left, std::pair<size_t, std::string> const& right) -> bool {
					return left.second.compare(right.second) < 0;
				});

				std::vector<string> names = vector<string>();
				std::vector<size_t> ids = vector<size_t>();
				names.resize(audioNames.size());
				ids.resize(audioNames.size());

				std::transform(audioNames.begin(), audioNames.end(), names.begin(), [](std::pair<size_t, string> const& i) -> string {
					return i.second;
				});

				std::transform(audioNames.begin(), audioNames.end(), ids.begin(), [](std::pair<size_t, string> const& i) -> size_t {
					return i.first;
				});

				audioNames.clear();

				names.insert(names.begin(), "None##Nothing");
				ids.insert(ids.begin(), 0);

				size_t choosed = 0;

				if (ImGui::ComboWithFilter(string("##Audio").append(id).c_str(), &choosed, names, 20)) {
					if (choosed != 0) {
						_audios.push_back(ids[choosed]);
					}
				}

				names.clear();
				ids.clear();

				if (choosed != 0) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	}
#endif
};