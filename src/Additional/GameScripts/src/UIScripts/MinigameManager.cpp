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
			//PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			//EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

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
			//PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			//EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

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
			//PlayerScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + playerWins));
			//EnemyScore->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes('0' + enemyWins));

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
		switch (player->minigameChoice) {
		case MinigameRPS_Choice::LAUNCHPAD:
			enemy->minigameChoice = MinigameRPS_Choice::GUITAR;
			enemyWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Launchpad");
			EnemySelectionImage->SetSprite("Guitar");
			break;

		case MinigameRPS_Choice::GUITAR:
			enemy->minigameChoice = MinigameRPS_Choice::DRUM;
			enemyWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Guitar");
			EnemySelectionImage->SetSprite("Drum");
			break;

		case MinigameRPS_Choice::DRUM:
			enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
			enemyWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Drum");
			EnemySelectionImage->SetSprite("Launchpad");
			break;
		}
	}
	else if (chance > (enemyLuckPoint + enemy->paperRockScisorsDrawLuck)) {
		switch (player->minigameChoice) {
		case MinigameRPS_Choice::LAUNCHPAD:
			enemy->minigameChoice = MinigameRPS_Choice::DRUM;
			playerWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Launchpad");
			EnemySelectionImage->SetSprite("Drum");
			break;

		case MinigameRPS_Choice::GUITAR:
			enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
			playerWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Guitar");
			EnemySelectionImage->SetSprite("Launchpad");
			break;

		case MinigameRPS_Choice::DRUM:
			enemy->minigameChoice = MinigameRPS_Choice::GUITAR;
			playerWins += 1;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Drum");
			EnemySelectionImage->SetSprite("Guitar");
			break;
		}
	}
	else {
		enemy->minigameChoice = player->minigameChoice;
		switch (player->minigameChoice) {
		case MinigameRPS_Choice::LAUNCHPAD:
			enemy->minigameChoice = MinigameRPS_Choice::LAUNCHPAD;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Launchpad");
			EnemySelectionImage->SetSprite("Launchpad");
			break;

		case MinigameRPS_Choice::GUITAR:
			enemy->minigameChoice = MinigameRPS_Choice::GUITAR;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Guitar");
			EnemySelectionImage->SetSprite("Guitar");
			break;

		case MinigameRPS_Choice::DRUM:
			enemy->minigameChoice = MinigameRPS_Choice::DRUM;
			PlayerSelectionImage->SetColor(glm::vec4(1.0f));
			EnemySelectionImage->SetColor(glm::vec4(1.0f));
			PlayerSelectionImage->SetSprite("Drum");
			EnemySelectionImage->SetSprite("Drum");
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

		PlayerImage->SetSprite(colors[player->colorIdx] + "B");
		EnemyImage->SetSprite(colors[enemy->colorIdx] + "B");
		PlayerFrontImage->SetSprite(colors[player->colorIdx] + "T");
		EnemyFrontImage->SetSprite(colors[enemy->colorIdx] + "T");

		player->fightingPlayable = enemy;

		MinigamePlain->SetActive(true);
		return;
	}



	int p1FieldsNumber = 3;
	int p2FieldsNumber = 3;
	for (int i = 0; i < 6; ++i) {
		if (hexTilesGOs[i] != nullptr) {
			owner = hexTilesGOs[i]->GetComponent<HexTile>()->ownerEntity;
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

		unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Text>();

		size_t choosed_1 = PlayerScore == nullptr ? 0 : PlayerScore->GetId();
		size_t choosed_2 = EnemyScore == nullptr ? 0 : EnemyScore->GetId();

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
					PlayerScore = static_cast<Text*>(items[choosed_1]);
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
					EnemyScore = static_cast<Text*>(items[choosed_2]);
				}
			}

			ImGui::EndCombo();
		}

		items.clear();
		items = SceneManager::GetComponentsOfType<Image>();

		choosed_1 = PlayerImage == nullptr ? 0 : PlayerImage->GetId();
		choosed_2 = PlayerFrontImage == nullptr ? 0 : PlayerFrontImage->GetId();
		size_t choosed_3 = PlayerSelectionImage == nullptr ? 0 : PlayerSelectionImage->GetId();
		size_t choosed_4 = EnemyImage == nullptr ? 0 : EnemyImage->GetId();
		size_t choosed_5 = EnemyFrontImage == nullptr ? 0 : EnemyFrontImage->GetId();
		size_t choosed_6 = EnemySelectionImage == nullptr ? 0 : EnemySelectionImage->GetId();

		if (ImGui::BeginCombo(string("PlayerImage##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_2 || item.second->GetId() == choosed_3 || 
					item.second->GetId() == choosed_4 || item.second->GetId() == choosed_5 || item.second->GetId() == choosed_6) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

					if (clicked) continue;

					choosed_1 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_1 != 0) {
					PlayerImage = static_cast<Image*>(items[choosed_1]);
				}
			}

			ImGui::EndCombo();
		}

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

		if (ImGui::BeginCombo(string("EnemyImage##").append(id).c_str(), choosed_4 == 0 ? "None" : items[choosed_4]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_2 ||
					item.second->GetId() == choosed_3 || item.second->GetId() == choosed_5 || item.second->GetId() == choosed_6) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_4)) {

					if (clicked) continue;

					choosed_4 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_4 != 0) {
					EnemyImage = static_cast<Image*>(items[choosed_4]);
				}
			}

			ImGui::EndCombo();
		}

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

		items.clear();
		items = SceneManager::GetComponentsOfType<Button>();

		choosed_1 = LaunchpadButton == nullptr ? 0 : LaunchpadButton->GetId();
		choosed_2 = GuitarButton == nullptr ? 0 : GuitarButton->GetId();
		choosed_3 = DrumButton == nullptr ? 0 : DrumButton->GetId();

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
					LaunchpadButton = static_cast<Button*>(items[choosed_1]);
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
					GuitarButton = static_cast<Button*>(items[choosed_2]);
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
					DrumButton = static_cast<Button*>(items[choosed_3]);
				}
			}

			ImGui::EndCombo();
		}

		items.clear();

		std::vector<GameObject*> objs = SceneManager::GetAllGameObjects();

		int choosed_obj = MinigamePlain == nullptr ? -1 : std::find(objs.begin(), objs.end(), MinigamePlain) - objs.begin();

		if (ImGui::BeginCombo(string("MinigamePlain##").append(id).c_str(), choosed_obj == -1 ? "None" : objs[choosed_obj]->GetName().c_str())) {

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
					MinigamePlain = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		choosed_obj = WonPanel == nullptr ? -1 : std::find(objs.begin(), objs.end(), WonPanel) - objs.begin();

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
					WonPanel = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		choosed_obj = LostPanel == nullptr ? -1 : std::find(objs.begin(), objs.end(), LostPanel) - objs.begin();

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
					LostPanel = objs[choosed_obj];
				}
			}

			ImGui::EndCombo();
		}

		unsigned int v = MaxNumberOfTurns;
		ImGui::DragUInt(string("MaxNumberOfTurns##").append(id).c_str(), &v, 1.f, 0, UINT_MAX);

		if (v != MaxNumberOfTurns) {
			MaxNumberOfTurns = v;
		}
	}
}
#endif