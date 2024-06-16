#include <UIScripts/MinigameManager.h>
#include <AreaTaking/HexTile.h>
#include <Tilemap/HexagonalTile.h>
#include <Player.h>
#include <list>
#include <core/Random.h>
#include <manager/SceneManager.h>
#include <string>
#include <codecvt>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Processes;
using namespace Twin2Engine::UI;

MinigameManager* MinigameManager::_instance = nullptr;

MinigameManager* MinigameManager::GetInstance()
{
	return _instance;
}

void MinigameManager::LaunchpadButtonOnClick()
{
	if (_waitCoroutine != nullptr) {
		delete _waitCoroutine;
		_waitCoroutine = nullptr;
	}

	_waitCoroutine = new Coroutine([&](bool* finish) 
	{
		_player->minigameChoice = MinigameRPS_Choice::NONE;
		_enemy->minigameChoice = MinigameRPS_Choice::NONE;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		_launchpadButton->GetGameObject()->SetActive(true);
		_guitarButton->GetGameObject()->SetActive(true);
		_drumButton->GetGameObject()->SetActive(true);

		_launchpadButton->SetInteractable(true);
		_guitarButton->SetInteractable(true);
		_drumButton->SetInteractable(true);

		Image* img = _launchpadButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		img = _guitarButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		img = _drumButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		_arrowLD->SetActive(true);
		_arrowGL->SetActive(true);
		_arrowDG->SetActive(true);

		if (_playerWins == _minNumberOfWins) {
			_audioComp->SetAudio(_winSound);
			_audioComp->Play();
			_wonPanel->SetActive(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			FinishMinigame(_player, _enemy);
			_wonPanel->SetActive(false);
		}
		else if (_enemyWins == _minNumberOfWins) {
			_audioComp->SetAudio(_defeatSound);
			_audioComp->Play();
			_lostPanel->SetActive(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			FinishMinigame(_enemy, _player);
			_lostPanel->SetActive(false);
		}
	});

	_player->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
	_audioComp->SetAudio(_launchpadSound);
	_audioComp->Play();

	_launchpadButton->SetInteractable(false);
	_guitarButton->SetInteractable(false);
	_drumButton->SetInteractable(false);
}

void MinigameManager::GuitarButtonOnClick()
{
	if (_waitCoroutine != nullptr) {
		delete _waitCoroutine;
		_waitCoroutine = nullptr;
	}

	_waitCoroutine = new Coroutine([this](bool* finish) 
	{
		_player->minigameChoice = MinigameRPS_Choice::NONE;
		_enemy->minigameChoice = MinigameRPS_Choice::NONE;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		_launchpadButton->GetGameObject()->SetActive(true);
		_guitarButton->GetGameObject()->SetActive(true);
		_drumButton->GetGameObject()->SetActive(true);

		_launchpadButton->SetInteractable(true);
		_guitarButton->SetInteractable(true);
		_drumButton->SetInteractable(true);

		Image* img = _launchpadButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		img = _guitarButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		img = _drumButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		_arrowLD->SetActive(true);
		_arrowGL->SetActive(true);
		_arrowDG->SetActive(true);

		if (_playerWins == _minNumberOfWins) {
			_audioComp->SetAudio(_winSound);
			_audioComp->Play();
			_wonPanel->SetActive(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			FinishMinigame(_player, _enemy);
			_wonPanel->SetActive(false);
		}
		else if (_enemyWins == _minNumberOfWins) {
			_audioComp->SetAudio(_defeatSound);
			_audioComp->Play();
			_lostPanel->SetActive(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			FinishMinigame(_enemy, _player);
			_lostPanel->SetActive(false);
		}
	});

	_player->minigameChoice = MinigameRPS_Choice::GUITAR;
	_audioComp->SetAudio(_guitarSound);
	_audioComp->Play();

	_launchpadButton->SetInteractable(false);
	_guitarButton->SetInteractable(false);
	_drumButton->SetInteractable(false);
}

void MinigameManager::DrumButtonOnClick()
{
	if (_waitCoroutine != nullptr) {
		delete _waitCoroutine;
		_waitCoroutine = nullptr;
	}

	_waitCoroutine = new Coroutine([this](bool* finish) 
	{
		_player->minigameChoice = MinigameRPS_Choice::NONE;
		_enemy->minigameChoice = MinigameRPS_Choice::NONE;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		_launchpadButton->GetGameObject()->SetActive(true);
		_guitarButton->GetGameObject()->SetActive(true);
		_drumButton->GetGameObject()->SetActive(true);

		_launchpadButton->SetInteractable(true);
		_guitarButton->SetInteractable(true);
		_drumButton->SetInteractable(true);

		Image* img = _launchpadButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		img = _guitarButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		img = _drumButton->GetGameObject()->GetComponent<Image>();
		if (img != nullptr) img->SetColor(glm::vec4(1.f));

		_arrowLD->SetActive(true);
		_arrowGL->SetActive(true);
		_arrowDG->SetActive(true);

		if (_playerWins == _minNumberOfWins) {
			_audioComp->SetAudio(_winSound);
			_audioComp->Play();
			_wonPanel->SetActive(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			FinishMinigame(_player, _enemy);
			_wonPanel->SetActive(false);
		}
		else if (_enemyWins == _minNumberOfWins) {
			_audioComp->SetAudio(_defeatSound);
			_audioComp->Play();
			_lostPanel->SetActive(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			FinishMinigame(_enemy, _player);
			_lostPanel->SetActive(false);
		}
	});

	_player->minigameChoice = MinigameRPS_Choice::DRUM;
	_audioComp->SetAudio(_drumSound);
	_audioComp->Play();

	_launchpadButton->SetInteractable(false);
	_guitarButton->SetInteractable(false);
	_drumButton->SetInteractable(false);
}

void MinigameManager::SetupButtons()
{
	if (_launchpadButton != nullptr) {
		_launchpadEvent = _launchpadButton->GetOnClickEvent() += [&]() -> void { LaunchpadButtonOnClick(); };
	}

	if (_guitarButton != nullptr) {
		_guitarButton->GetOnClickEvent() += [&]() -> void { GuitarButtonOnClick(); };
	}

	if (_drumButton != nullptr) {
		_drumButton->GetOnClickEvent() += [&]() -> void { DrumButtonOnClick(); };
	}
}

void MinigameManager::StartMinigame(Playable* chalanging, Playable* chalanged)
{
	chalanging->fightingPlayable = chalanged;
	chalanged->fightingPlayable = chalanging;

	HexTile* tile = chalanging->CurrTile;
	GameObject* hexTilesGOs[6];
	tile->GetMapHexTile()->tile->GetAdjacentGameObjects(hexTilesGOs);
	Playable* owner = nullptr;

	_player = dynamic_cast<Player*>(chalanging);
	if (_player != nullptr) {
		SPDLOG_INFO("Player has attacked Enemy!");
		for (int i = 0; i < 6; ++i) {
			if (hexTilesGOs[i] != nullptr) {
				owner = hexTilesGOs[i]->GetComponent<HexTile>()->ownerEntity;
				if (chalanging == owner) {
					_playerFieldsNumber += 1;
				}
				else if (chalanged == owner) {
					_enemyFieldsNumber += 1;
				}
			}
		}

		_enemy = (Enemy*)chalanged;
		_enemy->ChangeState(&Enemy::_fightingState);

		_player->fightingPlayable = _enemy;

		_playerImage->SetSprite(std::string("MiniGame_").append(_colors[_player->colorIdx]).append("_Back"));
		_enemyImage->SetSprite(std::string("MiniGame_").append(_colors[_enemy->colorIdx]).append("_Back"));

		_playerWins = 0;
		_enemyWins = 0;

		if (_playerScore != nullptr) _playerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::to_string(0)));
		if (_enemyScore != nullptr) _enemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::to_string(0)));

		_minigameCanvas->SetActive(true);
		return;
	}

	_player = dynamic_cast<Player*>(chalanged);
	if (_player != nullptr) {
		SPDLOG_INFO("Enemy has attacked Player!");
		for (int i = 0; i < 6; ++i) {
			if (hexTilesGOs[i] != nullptr) {
				owner = hexTilesGOs[i]->GetComponent<HexTile>()->ownerEntity;
				if (chalanging == owner) {
					_enemyFieldsNumber += 1;
				}
				else if (chalanged == owner) {
					_playerFieldsNumber += 1;
				}
			}
		}
		_enemy = (Enemy*)chalanging;

		_player->fightingPlayable = _enemy;

		_playerImage->SetSprite(std::string("MiniGame_").append(_colors[_player->colorIdx]).append("_Back"));
		_enemyImage->SetSprite(std::string("MiniGame_").append(_colors[_enemy->colorIdx]).append("_Back"));

		_playerWins = 0;
		_enemyWins = 0;

		if (_playerScore != nullptr) _playerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::to_string(0)));
		if (_enemyScore != nullptr) _enemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::to_string(0)));

		_minigameCanvas->SetActive(true);
		return;
	}


	SPDLOG_INFO("Enemy has attacked Enemy!");
	Enemy* e = (Enemy*)chalanged;
	e->ChangeState(&Enemy::_fightingState);
	chalanging->minigameChoice = static_cast<MinigameRPS_Choice>(Random::Range<int>(1, 4));
}

void MinigameManager::FinishMinigame(Playable* winner, Playable* looser)
{
	winner->WonPaperRockScissors(looser);
	_minigameCanvas->SetActive(false);
	_playerFieldsNumber = 0;
	_enemyFieldsNumber = 0;
	_playerWins = 0;
	_enemyWins = 0;

	if (_playerScore != nullptr) _playerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0'));
	if (_enemyScore != nullptr) _enemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0'));
}

void MinigameManager::PlayerWon() {

	Image* lose = nullptr;

	switch (_player->minigameChoice) {
		case MinigameRPS_Choice::LAUNCHPAD: {
			_enemy->minigameChoice = MinigameRPS_Choice::DRUM;

			lose = _drumButton->GetGameObject()->GetComponent<Image>();

			_drumButton->SetInteractable(false);
			_launchpadButton->SetInteractable(false);
			_guitarButton->GetGameObject()->SetActive(false);
			_arrowDG->SetActive(false);
			_arrowGL->SetActive(false);
			break;
		}

		case MinigameRPS_Choice::GUITAR: {
			_enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;

			lose = _launchpadButton->GetGameObject()->GetComponent<Image>();

			_guitarButton->SetInteractable(false);
			_launchpadButton->SetInteractable(false);
			_drumButton->GetGameObject()->SetActive(false);
			_arrowLD->SetActive(false);
			_arrowDG->SetActive(false);
			break;
		}


		case MinigameRPS_Choice::DRUM: {
			_enemy->minigameChoice = MinigameRPS_Choice::GUITAR;

			lose = _guitarButton->GetGameObject()->GetComponent<Image>();

			_drumButton->SetInteractable(false);
			_guitarButton->SetInteractable(false);
			_launchpadButton->GetGameObject()->SetActive(false);
			_arrowLD->SetActive(false);
			_arrowGL->SetActive(false);
			break;
		}

	}

	if (lose != nullptr) lose->SetColor(glm::vec4(glm::vec3(0.3f), 1.f));

	_playerWins += 1;

	if (_playerScore != nullptr) _playerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::to_string(_playerWins)));

	_waitCoroutine->Start();
}

void MinigameManager::PlayerDrawed() {

	Image* draw = nullptr;

	switch (_player->minigameChoice) {
		case MinigameRPS_Choice::LAUNCHPAD: {
			_enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;

			draw = _launchpadButton->GetGameObject()->GetComponent<Image>();

			_launchpadButton->SetInteractable(false);
			_guitarButton->GetGameObject()->SetActive(false);
			_drumButton->GetGameObject()->SetActive(false);
			_arrowDG->SetActive(false);
			_arrowGL->SetActive(false);
			_arrowLD->SetActive(false);
			break;
		}

		case MinigameRPS_Choice::GUITAR: {
			_enemy->minigameChoice = MinigameRPS_Choice::GUITAR;

			draw = _guitarButton->GetGameObject()->GetComponent<Image>();

			_guitarButton->SetInteractable(false);
			_launchpadButton->GetGameObject()->SetActive(false);
			_drumButton->GetGameObject()->SetActive(false);
			_arrowDG->SetActive(false);
			_arrowGL->SetActive(false);
			_arrowLD->SetActive(false);
			break;
		}

		case MinigameRPS_Choice::DRUM: {
			_enemy->minigameChoice = MinigameRPS_Choice::DRUM;

			draw = _drumButton->GetGameObject()->GetComponent<Image>();

			_drumButton->SetInteractable(false);
			_guitarButton->GetGameObject()->SetActive(false);
			_launchpadButton->GetGameObject()->SetActive(false);
			_arrowDG->SetActive(false);
			_arrowGL->SetActive(false);
			_arrowLD->SetActive(false);
			break;
		}
	}

	if (draw != nullptr) draw->SetColor(glm::vec4(glm::vec3(0.3f), 1.f));

	_waitCoroutine->Start();
}

void MinigameManager::PlayerLost() {

	Image* lose = nullptr;

	switch (_player->minigameChoice) {
		case MinigameRPS_Choice::LAUNCHPAD: {
			_enemy->minigameChoice = MinigameRPS_Choice::GUITAR;

			lose = _launchpadButton->GetGameObject()->GetComponent<Image>();

			_guitarButton->SetInteractable(false);
			_launchpadButton->SetInteractable(false);
			_drumButton->GetGameObject()->SetActive(false);
			_arrowDG->SetActive(false);
			_arrowLD->SetActive(false);
			break;
		}

		case MinigameRPS_Choice::GUITAR: {
			_enemy->minigameChoice = MinigameRPS_Choice::DRUM;

			lose = _guitarButton->GetGameObject()->GetComponent<Image>();

			_guitarButton->SetInteractable(false);
			_drumButton->SetInteractable(false);
			_launchpadButton->GetGameObject()->SetActive(false);
			_arrowGL->SetActive(false);
			_arrowLD->SetActive(false);
			break;
		}

		case MinigameRPS_Choice::DRUM: {
			_enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;

			lose = _drumButton->GetGameObject()->GetComponent<Image>();

			_drumButton->SetInteractable(false);
			_launchpadButton->SetInteractable(false);
			_guitarButton->GetGameObject()->SetActive(false);
			_arrowGL->SetActive(false);
			_arrowDG->SetActive(false);
			break;
		}
	}

	if (lose != nullptr) lose->SetColor(glm::vec4(glm::vec3(0.3f), 1.f));

	_enemyWins += 1;

	if (_enemyScore != nullptr) _enemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::to_string(_enemyWins)));

	_waitCoroutine->Start();
}

size_t MinigameManager::GetMinNumberOfWins()
{
	return _minNumberOfWins;
}

size_t MinigameManager::GetNumberOfWinsPlayer()
{
	return _playerWins;
}

size_t MinigameManager::GetNumberOfWinsEnemy()
{
	return _enemyWins;
}

void MinigameManager::Initialize()
{
	_instance = this;
	_minigameCanvas = GetGameObject();
	_minigameCanvas->SetActive(false);

	_audioComp = GetGameObject()->GetComponent<AudioComponent>();

	/*
	_winSound = 1;
	_defeatSound = 2;
	_guitarSound = 3;
	_drumSound = 4;
	_launchpadSound = 5;
	*/
}

void MinigameManager::OnDestroy()
{
	delete _waitCoroutine;
	_waitCoroutine = nullptr;

	if (_launchpadEvent != -1) {
		_launchpadButton->GetOnClickEvent() -= _launchpadEvent;
		_launchpadEvent = -1;
	}

	if (_guitarEvent != -1) {
		_guitarButton->GetOnClickEvent() -= _guitarEvent;
		_guitarEvent = -1;
	}

	if (_drumEvent != -1) {
		_drumButton->GetOnClickEvent() -= _drumEvent;
		_drumEvent = -1;
	}
}

YAML::Node MinigameManager::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "MinigameManager";
	node["MinNumberOfWins"] = _minNumberOfWins;
	node["PlayerScoreId"] = _playerScore == nullptr ? 0 : _playerScore->GetId();
	node["EnemyScoreId"] = _enemyScore == nullptr ? 0 : _enemyScore->GetId();
	node["PlayerImageId"] = _playerImage == nullptr ? 0 : _playerImage->GetId();
	//node["PlayerFrontImageId"] = PlayerFrontImage == nullptr ? 0 : PlayerFrontImage->GetId();
	//node["PlayerSelectionImageId"] = PlayerSelectionImage == nullptr ? 0 : PlayerSelectionImage->GetId();
	node["EnemyImageId"] = _enemyImage == nullptr ? 0 : _enemyImage->GetId();
	//node["EnemyFrontImageId"] = EnemyFrontImage == nullptr ? 0 : EnemyFrontImage->GetId();
	//node["EnemySelectionImageId"] = EnemySelectionImage == nullptr ? 0 : EnemySelectionImage->GetId();
	node["LaunchpadButtonId"] = _launchpadButton == nullptr ? 0 : _launchpadButton->GetId();
	node["GuitarButtonId"] = _guitarButton == nullptr ? 0 : _guitarButton->GetId();
	node["DrumButtonId"] = _drumButton == nullptr ? 0 : _drumButton->GetId();
	node["WonPanelId"] = _wonPanel == nullptr ? 0 : _wonPanel->Id();
	node["LostPanelId"] = _lostPanel == nullptr ? 0 : _lostPanel->Id();
	node["ArrowLDGameObject"] = _arrowLD == nullptr ? 0 : _arrowLD->Id();
	node["ArrowDGGameObject"] = _arrowDG == nullptr ? 0 : _arrowDG->Id();
	node["ArrowGLGameObject"] = _arrowGL == nullptr ? 0 : _arrowGL->Id();
	node["WinSound"] = SceneManager::GetAudioSaveIdx(_winSound);
	node["DefeatSound"] = SceneManager::GetAudioSaveIdx(_defeatSound);
	node["LaunchpadSound"] = SceneManager::GetAudioSaveIdx(_launchpadSound);
	node["GuitarSound"] = SceneManager::GetAudioSaveIdx(_guitarSound);
	node["DrumSound"] = SceneManager::GetAudioSaveIdx(_drumSound);

	return node;
}

bool MinigameManager::Deserialize(const YAML::Node& node)
{
	if (//!node["MaxNumberOfTurns"] ||
		!node["MinNumberOfWins"] ||
		!node["PlayerScoreId"] || !node["EnemyScoreId"] || 
		!node["PlayerImageId"] || 
		//!node["PlayerSelectionImageId"] || 
		!node["EnemyImageId"] ||
		//!node["EnemySelectionImageId"] || 
		!node["LaunchpadButtonId"] || !node["GuitarButtonId"] ||
		!node["DrumButtonId"] || !node["WonPanelId"] || !node["LostPanelId"] ||
		!node["ArrowLDGameObject"] || !node["ArrowDGGameObject"] || !node["ArrowGLGameObject"] ||
		!node["WinSound"] || !node["DefeatSound"] || !node["GuitarSound"] || !node["DrumSound"] || !node["LaunchpadSound"] ||
		!Component::Deserialize(node))
		return false;

	_minNumberOfWins = node["MinNumberOfWins"].as<size_t>();
	size_t id = node["PlayerScoreId"].as<size_t>();
	_playerScore = id == 0 ? nullptr : (Text*)SceneManager::GetComponentWithId(id);
	id = node["EnemyScoreId"].as<size_t>();
	_enemyScore = id == 0 ? nullptr : (Text*)SceneManager::GetComponentWithId(id);
	id = node["PlayerImageId"].as<size_t>();
	_playerImage = id == 0 ? nullptr : (Image*)SceneManager::GetComponentWithId(id);
	//id = node["PlayerFrontImageId"].as<size_t>();
	//_playerFrontImage = id == 0 ? nullptr : (Image*)SceneManager::GetComponentWithId(id);
	//id = node["PlayerSelectionImageId"].as<size_t>();
	//_playerSelectionImage = id == 0 ? nullptr : (Image*)SceneManager::GetComponentWithId(id);
	id = node["EnemyImageId"].as<size_t>();
	_enemyImage = id == 0 ? nullptr : (Image*)SceneManager::GetComponentWithId(id);
	//id = node["EnemyFrontImageId"].as<size_t>();
	//EnemyFrontImage = id == 0 ? nullptr : (Image*)SceneManager::GetComponentWithId(id);
	//id = node["EnemySelectionImageId"].as<size_t>();
	//EnemySelectionImage = id == 0 ? nullptr : (Image*)SceneManager::GetComponentWithId(id);
	id = node["LaunchpadButtonId"].as<size_t>();
	_launchpadButton = id == 0 ? nullptr : (Button*)SceneManager::GetComponentWithId(id);
	id = node["GuitarButtonId"].as<size_t>();
	_guitarButton = id == 0 ? nullptr : (Button*)SceneManager::GetComponentWithId(id);
	id = node["DrumButtonId"].as<size_t>();
	_drumButton = id == 0 ? nullptr : (Button*)SceneManager::GetComponentWithId(id);
	id = node["WonPanelId"].as<size_t>();
	_wonPanel = id == 0 ? nullptr : SceneManager::GetGameObjectWithId(id);
	id = node["LostPanelId"].as<size_t>();
	_lostPanel = id == 0 ? nullptr : SceneManager::GetGameObjectWithId(id);
	id = node["ArrowLDGameObject"].as<size_t>();
	_arrowLD = id == 0 ? nullptr : SceneManager::GetGameObjectWithId(id);
	id = node["ArrowDGGameObject"].as<size_t>();
	_arrowDG = id == 0 ? nullptr : SceneManager::GetGameObjectWithId(id);
	id = node["ArrowGLGameObject"].as<size_t>();
	_arrowGL = id == 0 ? nullptr : SceneManager::GetGameObjectWithId(id);

	_winSound = SceneManager::GetAudio(node["WinSound"].as<size_t>());
	_defeatSound = SceneManager::GetAudio(node["DefeatSound"].as<size_t>());
	_launchpadSound = SceneManager::GetAudio(node["LaunchpadSound"].as<size_t>());
	_guitarSound = SceneManager::GetAudio(node["GuitarSound"].as<size_t>());
	_drumSound = SceneManager::GetAudio(node["DrumSound"].as<size_t>());

	SetupButtons();

	return true;
}

#if _DEBUG
void MinigameManager::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("MinigameManager##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Text>();

		size_t choosed_1 = _playerScore == nullptr ? 0 : _playerScore->GetId();
		size_t choosed_2 = _enemyScore == nullptr ? 0 : _enemyScore->GetId();

		if (ImGui::BeginCombo(string("PlayerScore##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_2) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

					if (clicked) continue;

					choosed_1 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_1 != 0) {
					_playerScore = static_cast<Text*>(items[choosed_1]);
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo(string("EnemyScore##").append(id).c_str(), choosed_2 == 0 ? "None" : items[choosed_2]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_2)) {

					if (clicked) continue;

					choosed_2 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_2 != 0) {
					_enemyScore = static_cast<Text*>(items[choosed_2]);
				}
			}

			ImGui::EndCombo();
		}

		items.clear();
		items = SceneManager::GetComponentsOfType<Image>();

		choosed_1 = _playerImage == nullptr ? 0 : _playerImage->GetId();
		choosed_2 = _enemyImage == nullptr ? 0 : _enemyImage->GetId();
		//choosed_2 = PlayerFrontImage == nullptr ? 0 : PlayerFrontImage->GetId();
		//size_t choosed_3 = PlayerSelectionImage == nullptr ? 0 : PlayerSelectionImage->GetId();
		//size_t choosed_4 = EnemyImage == nullptr ? 0 : EnemyImage->GetId();
		//size_t choosed_5 = EnemyFrontImage == nullptr ? 0 : EnemyFrontImage->GetId();
		//size_t choosed_6 = EnemySelectionImage == nullptr ? 0 : EnemySelectionImage->GetId();

		if (ImGui::BeginCombo(string("PlayerImage##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_2 
					//|| item.second->GetId() == choosed_3 || item.second->GetId() == choosed_4 || item.second->GetId() == choosed_5 || item.second->GetId() == choosed_6
					) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

					if (clicked) continue;

					choosed_1 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_1 != 0) {
					_playerImage = static_cast<Image*>(items[choosed_1]);
				}
			}

			ImGui::EndCombo();
		}

		/*
		if (ImGui::BeginCombo(string("PlayerFrontImage##").append(id).c_str(), choosed_2 == 0 ? "None" : items[choosed_2]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_3 ||
					item.second->GetId() == choosed_4 || item.second->GetId() == choosed_5 || item.second->GetId() == choosed_6) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_2)) {

					if (clicked) continue;

					choosed_2 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_2 != 0) {
					PlayerFrontImage = static_cast<Image*>(items[choosed_2]);
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo(string("PlayerSelectionImage##").append(id).c_str(), choosed_3 == 0 ? "None" : items[choosed_3]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_2 ||
					item.second->GetId() == choosed_4 || item.second->GetId() == choosed_5 || item.second->GetId() == choosed_6) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_3)) {

					if (clicked) continue;

					choosed_3 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_3 != 0) {
					PlayerSelectionImage = static_cast<Image*>(items[choosed_3]);
				}
			}

			ImGui::EndCombo();
		}
		*/

		if (ImGui::BeginCombo(string("EnemyImage##").append(id).c_str(), choosed_2 == 0 ? "None" : items[choosed_2]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1
					//|| item.second->GetId() == choosed_2 || item.second->GetId() == choosed_3 || item.second->GetId() == choosed_5 || item.second->GetId() == choosed_6
					) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_2)) {

					if (clicked) continue;

					choosed_2 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_2 != 0) {
					_enemyImage = static_cast<Image*>(items[choosed_2]);
				}
			}

			ImGui::EndCombo();
		}

		/*
		if (ImGui::BeginCombo(string("EnemyFrontImage##").append(id).c_str(), choosed_5 == 0 ? "None" : items[choosed_5]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_2 ||
					item.second->GetId() == choosed_3 || item.second->GetId() == choosed_4 || item.second->GetId() == choosed_6) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_5)) {

					if (clicked) continue;

					choosed_5 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_5 != 0) {
					EnemyFrontImage = static_cast<Image*>(items[choosed_5]);
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo(string("EnemySelectionImage##").append(id).c_str(), choosed_6 == 0 ? "None" : items[choosed_6]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_2 ||
					item.second->GetId() == choosed_3 || item.second->GetId() == choosed_4 || item.second->GetId() == choosed_5) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_6)) {

					if (clicked) continue;

					choosed_6 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_6 != 0) {
					EnemySelectionImage = static_cast<Image*>(items[choosed_6]);
				}
			}

			ImGui::EndCombo();
		}
		*/

		items.clear();
		items = SceneManager::GetComponentsOfType<Button>();

		choosed_1 = _launchpadButton == nullptr ? 0 : _launchpadButton->GetId();
		choosed_2 = _guitarButton == nullptr ? 0 : _guitarButton->GetId();
		size_t choosed_3 = _drumButton == nullptr ? 0 : _drumButton->GetId();

		if (ImGui::BeginCombo(string("LaunchpadButton##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_2 || item.second->GetId() == choosed_3) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

					if (clicked) continue;

					choosed_1 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_1 != 0) {
					_launchpadButton = static_cast<Button*>(items[choosed_1]);
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo(string("GuitarButton##").append(id).c_str(), choosed_2 == 0 ? "None" : items[choosed_2]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_3) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_2)) {

					if (clicked) continue;

					choosed_2 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_2 != 0) {
					_guitarButton = static_cast<Button*>(items[choosed_2]);
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo(string("DrumButton##").append(id).c_str(), choosed_3 == 0 ? "None" : items[choosed_3]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_2) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_3)) {

					if (clicked) continue;

					choosed_3 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_3 != 0) {
					_drumButton = static_cast<Button*>(items[choosed_3]);
				}
			}

			ImGui::EndCombo();
		}

		items.clear();

		std::vector<GameObject*> objs = SceneManager::GetAllGameObjects();
		objs.erase(objs.begin());

		int choosed_obj = _minigameCanvas == nullptr ? -1 : std::find(objs.begin(), objs.end(), _minigameCanvas) - objs.begin();

		if (ImGui::BeginCombo(string("MinigameCanvas##").append(id).c_str(), choosed_obj == -1 ? "None" : objs[choosed_obj]->GetName().c_str())) {

			bool clicked = false;
			int i = 0;
			for (auto& item : objs) {

				if (ImGui::Selectable(std::string(item->GetName().c_str()).append("##").append(id).c_str(), i == choosed_obj)) {

					if (clicked) continue;

					choosed_obj = i;
					clicked = true;
				}

				++i;
			}

			if (clicked) {
				if (choosed_obj != -1) {
					_minigameCanvas = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		choosed_obj = _wonPanel == nullptr ? -1 : std::find(objs.begin(), objs.end(), _wonPanel) - objs.begin();

		if (ImGui::BeginCombo(string("WonPanel##").append(id).c_str(), choosed_obj == -1 ? "None" : objs[choosed_obj]->GetName().c_str())) {

			bool clicked = false;
			int i = 0;
			for (auto& item : objs) {

				if (ImGui::Selectable(std::string(item->GetName().c_str()).append("##").append(id).c_str(), i == choosed_obj)) {

					if (clicked) continue;

					choosed_obj = i;
					clicked = true;
				}

				++i;
			}

			if (clicked) {
				if (choosed_obj != -1) {
					_wonPanel = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		choosed_obj = _lostPanel == nullptr ? -1 : std::find(objs.begin(), objs.end(), _lostPanel) - objs.begin();

		if (ImGui::BeginCombo(string("LostPanel##").append(id).c_str(), choosed_obj == -1 ? "None" : objs[choosed_obj]->GetName().c_str())) {

			bool clicked = false;
			int i = 0;
			for (auto& item : objs) {

				if (ImGui::Selectable(std::string(item->GetName().c_str()).append("##").append(id).c_str(), i == choosed_obj)) {

					if (clicked) continue;

					choosed_obj = i;
					clicked = true;
				}

				++i;
			}

			if (clicked) {
				if (choosed_obj != -1) {
					_lostPanel = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		choosed_obj = _arrowLD == nullptr ? -1 : std::find(objs.begin(), objs.end(), _arrowLD) - objs.begin();

		if (ImGui::BeginCombo(string("ArrowLD##").append(id).c_str(), choosed_obj == -1 ? "None" : objs[choosed_obj]->GetName().c_str())) {

			bool clicked = false;
			int i = 0;
			for (auto& item : objs) {

				if (ImGui::Selectable(std::string(item->GetName().c_str()).append("##").append(id).c_str(), i == choosed_obj)) {

					if (clicked) continue;

					choosed_obj = i;
					clicked = true;
				}

				++i;
			}

			if (clicked) {
				if (choosed_obj != -1) {
					_arrowLD = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		choosed_obj = _arrowDG == nullptr ? -1 : std::find(objs.begin(), objs.end(), _arrowDG) - objs.begin();

		if (ImGui::BeginCombo(string("ArrowDG##").append(id).c_str(), choosed_obj == -1 ? "None" : objs[choosed_obj]->GetName().c_str())) {

			bool clicked = false;
			int i = 0;
			for (auto& item : objs) {

				if (ImGui::Selectable(std::string(item->GetName().c_str()).append("##").append(id).c_str(), i == choosed_obj)) {

					if (clicked) continue;

					choosed_obj = i;
					clicked = true;
				}

				++i;
			}

			if (clicked) {
				if (choosed_obj != -1) {
					_arrowDG = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		choosed_obj = _arrowGL == nullptr ? -1 : std::find(objs.begin(), objs.end(), _arrowGL) - objs.begin();

		if (ImGui::BeginCombo(string("ArrowGL##").append(id).c_str(), choosed_obj == -1 ? "None" : objs[choosed_obj]->GetName().c_str())) {

			bool clicked = false;
			int i = 0;
			for (auto& item : objs) {

				if (ImGui::Selectable(std::string(item->GetName().c_str()).append("##").append(id).c_str(), i == choosed_obj)) {

					if (clicked) continue;

					choosed_obj = i;
					clicked = true;
				}

				++i;
			}

			if (clicked) {
				if (choosed_obj != -1) {
					_arrowGL = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		objs.clear();

		unsigned int v = _minNumberOfWins;
		ImGui::DragUInt(string("MinNumberOfWins##").append(id).c_str(), &v, 1.f, 0, UINT_MAX);

		if (v != _minNumberOfWins) {
			_minNumberOfWins = v;
		}

		std::map<size_t, string> audioNames = AudioManager::GetAllAudiosNames();

		audioNames.insert(std::pair(0, "None"));

		if (!audioNames.contains(_winSound)) _winSound = 0;

		if (ImGui::BeginCombo(string("WinSound##").append(id).c_str(), audioNames[_winSound].c_str())) {

			bool clicked = false;
			size_t choosed = _winSound;
			for (auto& item : audioNames) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == _winSound)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				_winSound = choosed;
			}

			ImGui::EndCombo();
		}

		if (!audioNames.contains(_defeatSound)) _defeatSound = 0;

		if (ImGui::BeginCombo(string("DefeatSound##").append(id).c_str(), audioNames[_defeatSound].c_str())) {

			bool clicked = false;
			size_t choosed = _defeatSound;
			for (auto& item : audioNames) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == _defeatSound)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				_defeatSound = choosed;
			}

			ImGui::EndCombo();
		}

		if (!audioNames.contains(_launchpadSound)) _launchpadSound = 0;

		if (ImGui::BeginCombo(string("LaunchpadSound##").append(id).c_str(), audioNames[_launchpadSound].c_str())) {

			bool clicked = false;
			size_t choosed = _launchpadSound;
			for (auto& item : audioNames) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == _launchpadSound)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				_launchpadSound = choosed;
			}

			ImGui::EndCombo();
		}

		if (!audioNames.contains(_guitarSound)) _guitarSound = 0;

		if (ImGui::BeginCombo(string("GuitarSound##").append(id).c_str(), audioNames[_guitarSound].c_str())) {

			bool clicked = false;
			size_t choosed = _guitarSound;
			for (auto& item : audioNames) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == _guitarSound)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				_guitarSound = choosed;
			}

			ImGui::EndCombo();
		}

		if (!audioNames.contains(_drumSound)) _drumSound = 0;

		if (ImGui::BeginCombo(string("DrumSound##").append(id).c_str(), audioNames[_drumSound].c_str())) {

			bool clicked = false;
			size_t choosed = _drumSound;
			for (auto& item : audioNames) {

				if (ImGui::Selectable(std::string(item.second).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == _drumSound)) {

					if (clicked) continue;

					choosed = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				_drumSound = choosed;
			}

			ImGui::EndCombo();
		}

		audioNames.clear();
	}
}
#endif