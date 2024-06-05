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
	RockButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			RockButton->SetInteractable(false);
			PapperButton->SetInteractable(false);
			ScissorButton->SetInteractable(false);
			PerformTurn();
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			RockButton->SetInteractable(true);
			PapperButton->SetInteractable(true);
			ScissorButton->SetInteractable(true);
			PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

			PlayerSelectionImage->SetColor(glm::vec4(0.0));
			EnemySelectionImage->SetColor(glm::vec4(0.0));

			if (playerWins > (MaxNumberOfTurns / 2)) {
				FinishMinigame(player, enemy);
			}
			else if (enemyWins > (MaxNumberOfTurns / 2)) {
				FinishMinigame(enemy, player);
			}
			});

		playerChoice = Choice::LAUNCHPAD;
		waitCoroutine->Start();
		});

	PapperButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			RockButton->SetInteractable(false);
			PapperButton->SetInteractable(false);
			ScissorButton->SetInteractable(false);
			PerformTurn();
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			RockButton->SetInteractable(true);
			PapperButton->SetInteractable(true);
			ScissorButton->SetInteractable(true);
			PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

			PlayerSelectionImage->SetColor(glm::vec4(0.0));
			EnemySelectionImage->SetColor(glm::vec4(0.0));

			if (playerWins > (MaxNumberOfTurns / 2)) {
				FinishMinigame(player, enemy);
			}
			else if (enemyWins > (MaxNumberOfTurns / 2)) {
				FinishMinigame(enemy, player);
			}
			});

		playerChoice = Choice::GUITAR;
		waitCoroutine->Start();
		});

	ScissorButton->GetOnClickEvent().AddCallback([this]() {
		if (waitCoroutine != nullptr) {
			delete waitCoroutine;
		}
		waitCoroutine = new Coroutine([this](bool* finish) {
			RockButton->SetInteractable(false);
			PapperButton->SetInteractable(false);
			ScissorButton->SetInteractable(false);
			PerformTurn();
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			RockButton->SetInteractable(true);
			PapperButton->SetInteractable(true);
			ScissorButton->SetInteractable(true);
			PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

			PlayerSelectionImage->SetColor(glm::vec4(0.0));
			EnemySelectionImage->SetColor(glm::vec4(0.0));

			if (playerWins > (MaxNumberOfTurns / 2)) {
				FinishMinigame(player, enemy);
			}
			else if (enemyWins > (MaxNumberOfTurns / 2)) {
				FinishMinigame(enemy, player);
			}
			});

		playerChoice = Choice::DRUM;
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
	float enemyLuckPoint = .1f * (enemyFieldsNumber - playerFieldsNumber); // 1.0f;//

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
			PlayerSelectionImage->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
			EnemySelectionImage->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
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

		MinigamePlain->SetActive(true);
		return;
	}



	int p1FieldsNumber = 0;
	int p2FieldsNumber = 0;
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
	node["PlayerScoreId"] = 0;
	node["EnemyScoreId"] = 0;
	node["PlayerImageId"] = 0;
	node["PlayerSelectionImageId"] = 0;
	node["EnemyImageId"] = 0;
	node["EnemySelectionImageId"] = 0;
	node["RockButtonId"] = 0;
	node["PapperButtonId"] = 0;
	node["ScissorButtonId"] = 0;

	return node;
}

bool MinigameManager::Deserialize(const YAML::Node& node)
{
	if (!Component::Deserialize(node))
		return false;

	MaxNumberOfTurns = node["MaxNumberOfTurns"].as<int>();
	size_t id = node["PlayerScoreId"].as<int>();
	PlayerScore = (Text*)SceneManager::GetComponentWithId(id);
	id = node["EnemyScoreId"].as<int>();
	EnemyScore = (Text*)SceneManager::GetComponentWithId(id);
	id = node["PlayerImageId"].as<int>();
	PlayerImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["EnemyImageId"].as<int>();
	EnemyImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["PlayerSelectionImageId"].as<int>();
	PlayerSelectionImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["EnemySelectionImageId"].as<int>();
	EnemySelectionImage = (Image*)SceneManager::GetComponentWithId(id);
	id = node["RockButtonId"].as<int>();
	RockButton = (Button*)SceneManager::GetComponentWithId(id);
	id = node["PapperButtonId"].as<int>();
	PapperButton = (Button*)SceneManager::GetComponentWithId(id);
	id = node["ScissorButtonId"].as<int>();
	ScissorButton = (Button*)SceneManager::GetComponentWithId(id);

	SetupButtons();

	return true;
}

void MinigameManager::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("MinigameManager##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		//ImGui::DragFloat(string("ABSORPTION##").append(id).c_str(), &CloudController::ABSORPTION);
	}
}
