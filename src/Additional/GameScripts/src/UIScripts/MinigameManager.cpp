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

MinigameManager* MinigameManager::lastInstance = nullptr;

MinigameManager* MinigameManager::GetLastInstance()
{
	return lastInstance;
}

void MinigameManager::SetupButtons()
{
	LaunchpadButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			player->minigameChoice = MinigameRPS_Choice::NONE;
			enemy->minigameChoice = MinigameRPS_Choice::NONE;
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			LaunchpadButton->SetInteractable(true);
			GuitarButton->SetInteractable(true);
			DrumButton->SetInteractable(true);

			PlayerSelectionImage->SetColor(glm::vec4(0.0));
			EnemySelectionImage->SetColor(glm::vec4(0.0));

			if (playerWins > (MaxNumberOfTurns / 2)) {
				audioComp->SetAudio("res/music/Minigame/Win.mp3");
				audioComp->Play();
				WonPanel->SetActive(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				FinishMinigame(player, enemy);
				WonPanel->SetActive(false);
			}
			else if (enemyWins > (MaxNumberOfTurns / 2)) {
				audioComp->SetAudio("res/music/Minigame/Defeat.mp3");
				audioComp->Play();
				LostPanel->SetActive(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				FinishMinigame(enemy, player);
				LostPanel->SetActive(false);
			}
			});

		player->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
		PlayerSelectionImage->SetColor(glm::vec4(1.0f));
		PlayerSelectionImage->SetSprite("Launchpad");
		audioComp->SetAudio("res/music/Minigame/Launchpad.mp3");
		audioComp->Play();

		LaunchpadButton->SetInteractable(false);
		GuitarButton->SetInteractable(false);
		DrumButton->SetInteractable(false);
		});

	GuitarButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			player->minigameChoice = MinigameRPS_Choice::NONE;
			enemy->minigameChoice = MinigameRPS_Choice::NONE;
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			LaunchpadButton->SetInteractable(true);
			GuitarButton->SetInteractable(true);
			DrumButton->SetInteractable(true);

			PlayerSelectionImage->SetColor(glm::vec4(0.0));
			EnemySelectionImage->SetColor(glm::vec4(0.0));

			if (playerWins > (MaxNumberOfTurns / 2)) {
				audioComp->SetAudio("res/music/Minigame/Win.mp3");
				audioComp->Play();
				WonPanel->SetActive(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				FinishMinigame(player, enemy);
				WonPanel->SetActive(false);
			}
			else if (enemyWins > (MaxNumberOfTurns / 2)) {
				audioComp->SetAudio("res/music/Minigame/Defeat.mp3");
				audioComp->Play();
				LostPanel->SetActive(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				FinishMinigame(enemy, player);
				LostPanel->SetActive(false);
			}
			});

		player->minigameChoice = MinigameRPS_Choice::GUITAR;
		PlayerSelectionImage->SetColor(glm::vec4(1.0f));
		PlayerSelectionImage->SetSprite("Guitar");
		audioComp->SetAudio("res/music/Minigame/Guitar.mp3");
		audioComp->Play();

		LaunchpadButton->SetInteractable(false);
		GuitarButton->SetInteractable(false);
		DrumButton->SetInteractable(false);
		});

	DrumButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			player->minigameChoice = MinigameRPS_Choice::NONE;
			enemy->minigameChoice = MinigameRPS_Choice::NONE;
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			LaunchpadButton->SetInteractable(true);
			GuitarButton->SetInteractable(true);
			DrumButton->SetInteractable(true);

			PlayerSelectionImage->SetColor(glm::vec4(0.0));
			EnemySelectionImage->SetColor(glm::vec4(0.0));

			if (playerWins > (MaxNumberOfTurns / 2)) {
				audioComp->SetAudio("res/music/Minigame/Win.mp3");
				audioComp->Play();
				WonPanel->SetActive(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				FinishMinigame(player, enemy);
				WonPanel->SetActive(false);
			}
			else if (enemyWins > (MaxNumberOfTurns / 2)) {
				audioComp->SetAudio("res/music/Minigame/Defeat.mp3");
				audioComp->Play();
				LostPanel->SetActive(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				FinishMinigame(enemy, player);
				LostPanel->SetActive(false);
			}
			});

		player->minigameChoice = MinigameRPS_Choice::DRUM;
		PlayerSelectionImage->SetColor(glm::vec4(1.0f));
		PlayerSelectionImage->SetSprite("Drum");
		audioComp->SetAudio("res/music/Minigame/Drum.mp3");
		audioComp->Play();

		LaunchpadButton->SetInteractable(false);
		GuitarButton->SetInteractable(false);
		DrumButton->SetInteractable(false);
		});
}

void MinigameManager::StartMinigame(Playable* chalanging, Playable* chalanged)
{
	chalanging->fightingPlayable = chalanged;
	chalanged->fightingPlayable = chalanging;

	HexTile* tile = chalanging->CurrTile;
	GameObject* hexTilesGOs[6];
	tile->GetMapHexTile()->tile->GetAdjacentGameObjects(hexTilesGOs);
	Playable* owner = nullptr;

	player = dynamic_cast<Player*>(chalanging);
	if (player != nullptr) {
		SPDLOG_INFO("Player has attacked Enemy!");
		for (int i = 0; i < 6; ++i) {
			if (hexTilesGOs[i] != nullptr) {
				owner = hexTilesGOs[i]->GetComponent<HexTile>()->ownerEntity;
				if (chalanging == owner) {
					playerFieldsNumber += 1;
				}
				else if (chalanged == owner) {
					enemyFieldsNumber += 1;
				}
			}
		}

		enemy = (Enemy*)chalanged;
		enemy->ChangeState(&Enemy::_fightingState);

		player->fightingPlayable = enemy;

		PlayerImage->SetSprite(colors[player->colorIdx] + "B");
		EnemyImage->SetSprite(colors[enemy->colorIdx] + "B");
		PlayerFrontImage->SetSprite(colors[player->colorIdx] + "T");
		EnemyFrontImage->SetSprite(colors[enemy->colorIdx] + "T");

		MinigamePlain->SetActive(true);
		return;
	}

	player = dynamic_cast<Player*>(chalanged);
	if (player != nullptr) {
		SPDLOG_INFO("Enemy has attacked Player!");
		for (int i = 0; i < 6; ++i) {
			if (hexTilesGOs[i] != nullptr) {
				owner = hexTilesGOs[i]->GetComponent<HexTile>()->ownerEntity;
				if (chalanging == owner) {
					enemyFieldsNumber += 1;
				}
				else if (chalanged == owner) {
					playerFieldsNumber += 1;
				}
			}
		}
		enemy = (Enemy*)chalanging;
		//enemy->ChangeState(&Enemy::_fightingState);

		PlayerImage->SetSprite(colors[player->colorIdx] + "B");
		EnemyImage->SetSprite(colors[enemy->colorIdx] + "B");
		PlayerFrontImage->SetSprite(colors[player->colorIdx] + "T");
		EnemyFrontImage->SetSprite(colors[enemy->colorIdx] + "T");

		player->fightingPlayable = enemy;

		MinigamePlain->SetActive(true);
		return;
	}


	SPDLOG_INFO("Enemy has attacked Enemy!");
	Enemy* e = (Enemy*)chalanged;
	e->ChangeState(&Enemy::_fightingState);
	chalanging->minigameChoice = static_cast<MinigameRPS_Choice>(Random::Range<int>(1, 4));
}

void MinigameManager::PlayerWon() {
	switch (player->minigameChoice) {
	case MinigameRPS_Choice::LAUNCHPAD:
		enemy->minigameChoice = MinigameRPS_Choice::DRUM;
		EnemySelectionImage->SetSprite("Drum");
		break;

	case MinigameRPS_Choice::GUITAR:
		enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
		EnemySelectionImage->SetSprite("Launchpad");
		break;

	case MinigameRPS_Choice::DRUM:
		enemy->minigameChoice = MinigameRPS_Choice::GUITAR;
		EnemySelectionImage->SetSprite("Guitar");
		break;
	}

	playerWins += 1;
	EnemySelectionImage->SetColor(glm::vec4(1.0f));

	waitCoroutine->Start();
}

void MinigameManager::PlayerDrawed() {
	switch (player->minigameChoice) {
	case MinigameRPS_Choice::LAUNCHPAD:
		enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
		EnemySelectionImage->SetSprite("Launchpad");
		break;

	case MinigameRPS_Choice::GUITAR:
		enemy->minigameChoice = MinigameRPS_Choice::GUITAR;
		EnemySelectionImage->SetSprite("Guitar");
		break;

	case MinigameRPS_Choice::DRUM:
		enemy->minigameChoice = MinigameRPS_Choice::DRUM;
		EnemySelectionImage->SetSprite("Drum");
		break;
	}

	EnemySelectionImage->SetColor(glm::vec4(1.0f));

	waitCoroutine->Start();
}

void MinigameManager::PlayerLost() {
	switch (player->minigameChoice) {
	case MinigameRPS_Choice::LAUNCHPAD:
		enemy->minigameChoice = MinigameRPS_Choice::GUITAR;
		EnemySelectionImage->SetSprite("Guitar");
		break;

	case MinigameRPS_Choice::GUITAR:
		enemy->minigameChoice = MinigameRPS_Choice::DRUM;
		EnemySelectionImage->SetSprite("Drum");
		break;

	case MinigameRPS_Choice::DRUM:
		enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
		EnemySelectionImage->SetSprite("Launchpad");
		break;
	}

	enemyWins += 1;
	EnemySelectionImage->SetColor(glm::vec4(1.0f));

	waitCoroutine->Start();
}


void MinigameManager::FinishMinigame(Playable* winner, Playable* looser)
{
	winner->WonPaperRockScissors(looser);
	//looser->LostPaperRockScissors(winner);
	MinigamePlain->SetActive(false);
	playerFieldsNumber = 0;
	enemyFieldsNumber = 0;
	playerWins = 0;
	enemyWins = 0;
	PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0'));
	EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0'));
}


void MinigameManager::Initialize()
{
	lastInstance = this;
	MinigamePlain = GetGameObject();
	MinigamePlain->SetActive(false);
	//std::list<Button*> buttons = MinigamePlain->GetComponentsInChildren<Button>();
	audioComp = GetGameObject()->GetComponent<AudioComponent>();
	

	WinSound		= 1;
	DefeatSound		= 2;
	GuitarSound		= 3;
	DrumSound		= 4;
	LaunchpadSound	= 5;
}

void MinigameManager::OnEnable()
{

}

void MinigameManager::OnDestroy()
{
	delete waitCoroutine;
}

void MinigameManager::Update()
{

}

YAML::Node MinigameManager::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "MinigameManager";
	node["MaxNumberOfTurns"] = MaxNumberOfTurns;
	node["PlayerScoreId"] = PlayerScore->GetId();
	node["EnemyScoreId"] = EnemyScore->GetId();
	node["PlayerImageId"] = PlayerImage->GetId();
	node["PlayerFrontImageId"] = PlayerFrontImage->GetId();
	node["PlayerSelectionImageId"] = PlayerSelectionImage->GetId();
	node["EnemyImageId"] = EnemyImage->GetId();
	node["EnemyFrontImageId"] = EnemyFrontImage->GetId();
	node["EnemySelectionImageId"] = EnemySelectionImage->GetId();
	node["LaunchpadButtonId"] = LaunchpadButton->GetId();
	node["GuitarButtonId"] = GuitarButton->GetId();
	node["DrumButtonId"] = DrumButton->GetId();
	node["WonPanelId"] = WonPanel->Id();
	node["LostPanelId"] = LostPanel->Id();

	return node;
}

bool MinigameManager::Deserialize(const YAML::Node& node)
{
	if (!node["MaxNumberOfTurns"] || !node["PlayerScoreId"] || !node["EnemyScoreId"] || 
		!node["PlayerImageId"] || !node["PlayerSelectionImageId"] || !node["EnemyImageId"] ||
		!node["EnemySelectionImageId"] || !node["LaunchpadButtonId"] || !node["GuitarButtonId"] ||
		!node["DrumButtonId"] || !node["WonPanelId"] || !node["LostPanelId"] ||
		!Component::Deserialize(node))
		return false;

	MaxNumberOfTurns = node["MaxNumberOfTurns"].as<size_t>();
	size_t id = node["PlayerScoreId"].as<size_t>();
	PlayerScore = (Text*)SceneManager::GetComponentWithId(id);
	id = node["EnemyScoreId"].as<size_t>();
	EnemyScore = (Text*)SceneManager::GetComponentWithId(id);
	id = node["PlayerImageId"].as<size_t>();
	PlayerImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["PlayerFrontImageId"].as<size_t>();
	PlayerFrontImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["PlayerSelectionImageId"].as<size_t>();
	PlayerSelectionImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["EnemyImageId"].as<size_t>();
	EnemyImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["EnemyFrontImageId"].as<size_t>();
	EnemyFrontImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["EnemySelectionImageId"].as<size_t>();
	EnemySelectionImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["LaunchpadButtonId"].as<size_t>();
	LaunchpadButton = (Button*)SceneManager::GetComponentWithId(id);
	id = node["GuitarButtonId"].as<size_t>();
	GuitarButton = (Button*)SceneManager::GetComponentWithId(id);
	id = node["DrumButtonId"].as<size_t>();
	DrumButton = (Button*)SceneManager::GetComponentWithId(id);
	id = node["WonPanelId"].as<size_t>();
	WonPanel = SceneManager::GetGameObjectWithId(id);
	id = node["LostPanelId"].as<size_t>();
	LostPanel = SceneManager::GetGameObjectWithId(id);

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

		//ImGui::DragFloat(string("ABSORPTION##").append(id).c_str(), &CloudController::ABSORPTION);
	}
}
#endif