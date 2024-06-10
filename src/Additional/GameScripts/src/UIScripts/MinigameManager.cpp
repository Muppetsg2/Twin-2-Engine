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
			LaunchpadButton->SetInteractable(false);
			GuitarButton->SetInteractable(false);
			DrumButton->SetInteractable(false);
			PerformTurn();
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			LaunchpadButton->SetInteractable(true);
			GuitarButton->SetInteractable(true);
			DrumButton->SetInteractable(true);
			PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

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

		playerChoice = Choice::LAUNCHPAD;
		audioComp->SetAudio("res/music/Minigame/Launchpad.mp3");
		audioComp->Play();

		waitCoroutine->Start();
		});

	GuitarButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			LaunchpadButton->SetInteractable(false);
			GuitarButton->SetInteractable(false);
			DrumButton->SetInteractable(false);
			PerformTurn();
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			LaunchpadButton->SetInteractable(true);
			GuitarButton->SetInteractable(true);
			DrumButton->SetInteractable(true);
			PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

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

		playerChoice = Choice::GUITAR;
		audioComp->SetAudio("res/music/Minigame/Guitar.mp3");
		audioComp->Play();
		waitCoroutine->Start();
		});

	DrumButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			LaunchpadButton->SetInteractable(false);
			GuitarButton->SetInteractable(false);
			DrumButton->SetInteractable(false);
			PerformTurn();
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			LaunchpadButton->SetInteractable(true);
			GuitarButton->SetInteractable(true);
			DrumButton->SetInteractable(true);
			PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

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

		playerChoice = Choice::DRUM;
		audioComp->SetAudio("res/music/Minigame/Drum.mp3");
		audioComp->Play();
		waitCoroutine->Start();
		});
}

void MinigameManager::PerformTurn()
{
	//float chanceForEnemyWin = 66.0f * (float)enemyFieldsNumber / (float)(enemyFieldsNumber + playerFieldsNumber);
	//float chanceForEnemyLost = 66.0f * (float)playerFieldsNumber / (float)(enemyFieldsNumber + playerFieldsNumber);
	//float percantage = Random::Range(0.0f, 100.0f);
	float chance = Random::Range(0.0f, 1.0f);
	float drawchance = Random::Range(0.0f, 1.0f);
	float enemyLuckPoint = .1f * (enemyFieldsNumber - playerFieldsNumber) - 1.0f; // 1.0f;//

	//else if (percantage >= (100.0f - chanceForEnemyLost)) {
	//if (percantage <= chanceForEnemyWin) {
	if (chance <= (enemyLuckPoint + enemy->paperRockScisorsWinLuck)) {
		switch (playerChoice) {
		case Choice::LAUNCHPAD:
			enemyChoice = Choice::GUITAR;
			enemyWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("launchpad");
			EnemySelectionImage->SetSprite("guitar");
			break;

		case Choice::GUITAR:
			enemyChoice = Choice::DRUM;
			enemyWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("guitar");
			EnemySelectionImage->SetSprite("drum");
			break;

		case Choice::DRUM:
			enemyChoice = Choice::LAUNCHPAD;
			enemyWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("drum");
			EnemySelectionImage->SetSprite("launchpad");
			break;
		}
	}
	else if (chance > (enemyLuckPoint + enemy->paperRockScisorsDrawLuck)) {
		switch (playerChoice) {
		case Choice::LAUNCHPAD:
			enemyChoice = Choice::DRUM;
			playerWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("launchpad");
			EnemySelectionImage->SetSprite("drum");
			break;

		case Choice::GUITAR:
			enemyChoice = Choice::LAUNCHPAD;
			playerWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("guitar");
			EnemySelectionImage->SetSprite("launchpad");
			break;

		case Choice::DRUM:
			enemyChoice = Choice::GUITAR;
			playerWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("drum");
			EnemySelectionImage->SetSprite("guitar");
			break;
		}
	}
	else {
		enemyChoice = playerChoice;
		switch (playerChoice) {
		case Choice::LAUNCHPAD:
			enemyChoice = Choice::LAUNCHPAD;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("launchpad");
			EnemySelectionImage->SetSprite("launchpad");
			break;

		case Choice::GUITAR:
			enemyChoice = Choice::GUITAR;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("guitar");
			EnemySelectionImage->SetSprite("guitar");
			break;

		case Choice::DRUM:
			enemyChoice = Choice::DRUM;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("drum");
			EnemySelectionImage->SetSprite("drum");
			break;
		}
	}

}

void MinigameManager::StartMinigame(Playable* chalanging, Playable* chalanged)
{
	HexTile* tile = chalanging->CurrTile;
	GameObject* hexTilesGOs[6];
	tile->GetMapHexTile()->tile->GetAdjacentGameObjects(hexTilesGOs);
	Playable* owner = nullptr;

	player = dynamic_cast<Player*>(chalanging);
	if (player != nullptr) {
		for (int i = 0; i < 6; ++i) {
			if (hexTilesGOs[i] != nullptr) {
				owner = hexTilesGOs[i]->GetComponent<HexTile>()->takenEntity;
				if (chalanging == owner) {
					playerFieldsNumber += 1;
				}
				else if (chalanged == owner) {
					enemyFieldsNumber += 1;
				}
			}
		}

		enemy = (Enemy*)chalanged;

		MinigamePlain->SetActive(true);
		return;
	}

	player = dynamic_cast<Player*>(chalanged);
	if (player != nullptr) {
		for (int i = 0; i < 6; ++i) {
			if (hexTilesGOs[i] != nullptr) {
				owner = hexTilesGOs[i]->GetComponent<HexTile>()->takenEntity;
				if (chalanging == owner) {
					enemyFieldsNumber += 1;
				}
				else if (chalanged == owner) {
					playerFieldsNumber += 1;
				}
			}
		}
		enemy = (Enemy*)chalanging;

		player->fightingPlayable = enemy;

		MinigamePlain->SetActive(true);
		return;
	}



	int p1FieldsNumber = 3;
	int p2FieldsNumber = 3;
	for (int i = 0; i < 6; ++i) {
		if (hexTilesGOs[i] != nullptr) {
			owner = hexTilesGOs[i]->GetComponent<HexTile>()->takenEntity;
			if (chalanging == owner) {
				p1FieldsNumber += 1;
			}
			else if (chalanged == owner) {
				p2FieldsNumber += 1;
			}
		}
	}

	int totalNumber = p1FieldsNumber + p2FieldsNumber;
	if (Random::Range(0.0f, (float)totalNumber) <= (float)p1FieldsNumber) {
		chalanging->WonPaperRockScissors(chalanged);
		//chalanged->LostPaperRockScissors(chalanging);
	}
	else {
		//chalanging->LostPaperRockScissors(chalanged);
		chalanged->WonPaperRockScissors(chalanging);
	}
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
	node["PlayerSelectionImageId"] = PlayerSelectionImage->GetId();
	node["EnemyImageId"] = EnemyImage->GetId();
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
	id = node["PlayerSelectionImageId"].as<size_t>();
	PlayerSelectionImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["EnemyImageId"].as<size_t>();
	EnemyImage = (Image*)SceneManager::GetComponentWithId(id);
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