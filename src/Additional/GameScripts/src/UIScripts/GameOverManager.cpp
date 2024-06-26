#include <UIScripts/GameOverManager.h>
#include <manager/SceneManager.h>
#include <GameManager.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;

GameOverManager* GameOverManager::instance = nullptr;

void GameOverManager::NewGame()
{
	SceneManager::LoadScene("Game");
}

void GameOverManager::Menu()
{
	SceneManager::LoadScene("Menu");
}

void GameOverManager::UpdateTexts() {
	GameTimer* timer = GameTimer::Instance();
	if (timer != nullptr) {
		if (_timeText != nullptr) {
			_timeText->SetText(to_wstring(timer->GetYears())
				.append(timer->GetYears() == 1 ? L" YEAR " : L" YEARS ")
				.append(to_wstring(timer->GetMonths()))
				.append(timer->GetMonths() == 1 ? L" MONTH " : L" MONTHS ")
				.append(L" AND ")
				.append(to_wstring(timer->GetDays()))
				.append(timer->GetDays() == 1 ? L" DAY" : L" DAYS")
			);
		}
	}

	Player* player = GameManager::instance->GetPlayer();
	if (player != nullptr) {
		if (_albumsNumText != nullptr) {
			_albumsNumText->SetText(to_wstring(player->albumUsed));
		}

		if (_concertsNumText != nullptr) {
			_concertsNumText->SetText(to_wstring(player->concertUsed));
		}

		if (_fansNumText != nullptr) {
			_fansNumText->SetText(to_wstring(player->fansUsed));
		}
	}
}

void GameOverManager::InitializeButtons()
{
	if (_newGameButtonEvent == -1 && _newGameButton != nullptr) _newGameButtonEvent = _newGameButton->GetOnClickEvent() += [&]() -> void { NewGame(); };
	if (_menuButtonEvent == -1 && _menuButton != nullptr) _menuButtonEvent = _menuButton->GetOnClickEvent() += [&]() -> void { Menu(); };
}

void GameOverManager::InitializeGameOverCanvas()
{
	if (_gameOverCanvasActiveChangeEvent == -1 && _gameOverCanvas != nullptr) _gameOverCanvasActiveChangeEvent = _gameOverCanvas->OnActiveChangedEvent += [&](GameObject* obj) -> void { if (obj->GetActive()) UpdateTexts(); };
}

void GameOverManager::Open()
{
	if (_gameOverCanvas != nullptr) {
		_gameOverCanvas->SetActive(true);
	}
}

void GameOverManager::Initialize()
{
	if (instance != nullptr) {
		SceneManager::DestroyGameObject(GetGameObject());
		return;
	}

	instance = this;

	InitializeButtons();
	InitializeGameOverCanvas();

	if (_gameOverCanvas != nullptr) { _gameOverCanvas->SetActive(false);	}
}

void GameOverManager::Update() {}

void GameOverManager::OnDestroy()
{
	if (instance == this) instance = nullptr;

	if (_gameOverCanvas != nullptr && _gameOverCanvasActiveChangeEvent != -1) {
		_gameOverCanvas->OnActiveChangedEvent -= _gameOverCanvasActiveChangeEvent;
		_gameOverCanvasActiveChangeEvent = -1;
	}

	if (_newGameButton != nullptr && _newGameButtonEvent != -1) {
		_newGameButton->GetOnClickEvent() -= _newGameButtonEvent;
		_newGameButtonEvent = -1;
	}

	if (_menuButton != nullptr && _menuButtonEvent != -1) {
		_menuButton->GetOnClickEvent() -= _menuButtonEvent;
		_menuButtonEvent = -1;
	}
}

YAML::Node GameOverManager::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "GameOverManager";
	node["gameOverCanvas"] = _gameOverCanvas != nullptr ? _gameOverCanvas->Id() : 0;
	node["newGameButtonId"] = _newGameButton != nullptr ? _newGameButton->GetId() : 0;
	node["menuButtonId"] = _menuButton != nullptr ? _menuButton->GetId() : 0;
	node["timeTextId"] = _timeText != nullptr ? _timeText->GetId() : 0;
	node["albumsNumTextId"] = _albumsNumText != nullptr ? _albumsNumText->GetId() : 0;
	node["concertsNumTextId"] = _concertsNumText != nullptr ? _concertsNumText->GetId() : 0;
	node["fansNumTextId"] = _fansNumText != nullptr ? _fansNumText->GetId() : 0;

	return node;
}

bool GameOverManager::Deserialize(const YAML::Node& node)
{
	bool goodData = Component::Deserialize(node);

	size_t id;
	if (node["gameOverCanvas"]) {
		id = node["gameOverCanvas"].as<size_t>();
		_gameOverCanvas = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;
	}
	goodData = goodData && node["gameOverCanvas"];

	if (node["newGameButtonId"]) {
		id = node["newGameButtonId"].as<size_t>();
		_newGameButton = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;
	}
	goodData = goodData && node["newGameButtonId"];

	if (node["menuButtonId"]) {
		id = node["menuButtonId"].as<size_t>();
		_menuButton = id != 0 ? (Button*)SceneManager::GetComponentWithId(id) : nullptr;
	}
	goodData = goodData && node["menuButtonId"];

	if (node["timeTextId"]) {
		id = node["timeTextId"].as<size_t>();
		_timeText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;
	}
	goodData = goodData && node["timeTextId"];

	if (node["albumsNumTextId"]) {
		id = node["albumsNumTextId"].as<size_t>();
		_albumsNumText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;
	}
	goodData = goodData && node["albumsNumTextId"];

	if (node["concertsNumTextId"]) {
		id = node["concertsNumTextId"].as<size_t>();
		_concertsNumText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;
	}
	goodData = goodData && node["concertsNumTextId"];

	if (node["fansNumTextId"]) {
		id = node["fansNumTextId"].as<size_t>();
		_fansNumText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;
	}
	goodData = goodData && node["fansNumTextId"];

	return goodData;
}

#if _DEBUG
void GameOverManager::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("GameOverManager##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		std::vector<GameObject*> objs = SceneManager::GetAllGameObjects();
		objs.erase(objs.begin());
		objs.insert(objs.begin(), nullptr);
		size_t choosed_1 = std::find(objs.begin(), objs.end(), _gameOverCanvas) - objs.begin();

		if (ImGui::BeginCombo(string("GameOverCanvas##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

			bool clicked = false;
			for (size_t i = 0; i < objs.size(); ++i) {

				if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

					if (clicked) continue;

					choosed_1 = i;
					clicked = true;
				}
			}

			if (clicked) {
				if (_gameOverCanvas != nullptr && _gameOverCanvasActiveChangeEvent != -1) {
					_gameOverCanvas->OnActiveChangedEvent -= _gameOverCanvasActiveChangeEvent;
					_gameOverCanvasActiveChangeEvent = -1;
				}

				_gameOverCanvas = objs[choosed_1];

				InitializeGameOverCanvas();
			}

			ImGui::EndCombo();
		}

		objs.clear();

		unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Button>();
		choosed_1 = _newGameButton == nullptr ? 0 : _newGameButton->GetId();

		if (ImGui::BeginCombo(string("NewGameButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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

					if (_newGameButton != nullptr) {
						if (_newGameButtonEvent != -1) {
							_newGameButton->GetOnClickEvent() -= _newGameButtonEvent;
							_newGameButtonEvent = -1;
						}
					}

					_newGameButton = static_cast<Button*>(items[choosed_1]);

					if (_newGameButton != nullptr) {
						InitializeButtons();
					}
				}
			}

			ImGui::EndCombo();
		}

		choosed_1 = _menuButton == nullptr ? 0 : _menuButton->GetId();

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

					if (_menuButton != nullptr) {
						if (_menuButtonEvent != -1) {
							_menuButton->GetOnClickEvent() -= _menuButtonEvent;
							_menuButtonEvent = -1;
						}
					}

					_menuButton = static_cast<Button*>(items[choosed_1]);

					if (_menuButton != nullptr) {
						InitializeButtons();
					}
				}
			}

			ImGui::EndCombo();
		}

		items.clear();

		items = SceneManager::GetComponentsOfType<Text>();
		choosed_1 = _timeText == nullptr ? 0 : _timeText->GetId();

		if (ImGui::BeginCombo(string("TimeText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
					_timeText = static_cast<Text*>(items[choosed_1]);

					if (_timeText != nullptr) {
						UpdateTexts();
					}
				}
			}

			ImGui::EndCombo();
		}

		choosed_1 = _albumsNumText == nullptr ? 0 : _albumsNumText->GetId();

		if (ImGui::BeginCombo(string("AlbumNumsText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
					_albumsNumText = static_cast<Text*>(items[choosed_1]);

					if (_albumsNumText != nullptr) {
						UpdateTexts();
					}
				}
			}

			ImGui::EndCombo();
		}

		choosed_1 = _concertsNumText == nullptr ? 0 : _concertsNumText->GetId();

		if (ImGui::BeginCombo(string("ConcertsNumText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
					_concertsNumText = static_cast<Text*>(items[choosed_1]);

					if (_concertsNumText != nullptr) {
						UpdateTexts();
					}
				}
			}

			ImGui::EndCombo();
		}

		choosed_1 = _fansNumText == nullptr ? 0 : _fansNumText->GetId();

		if (ImGui::BeginCombo(string("FansNumText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
					_fansNumText = static_cast<Text*>(items[choosed_1]);

					if (_fansNumText != nullptr) {
						UpdateTexts();
					}
				}
			}

			ImGui::EndCombo();
		}

		items.clear();
	}
}
#endif