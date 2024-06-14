#pragma once

#include <core/GameObject.h>
#include <core/Component.h>
#include <ui/Image.h>
#include <ui/Text.h>
#include <ui/Button.h>
#include <Playable.h>
#include <Player.h>
#include <Enemy.h>
#include <processes/Coroutine.h>

#include <manager/AudioManager.h>
#include <core/AudioComponent.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Processes;
using namespace Twin2Engine::UI;

// TODO: Repair
class MinigameManager : public Component {
	private:
		static MinigameManager* lastInstance;
		//MinigameRPS_Choice playerChoice = MinigameRPS_Choice::NONE;
		//MinigameRPS_Choice enemyChoice = MinigameRPS_Choice::NONE;
		int playerWins = 0;
		int enemyWins = 0;

		Player* player = nullptr;
		Enemy* enemy = nullptr;

		Coroutine* waitCoroutine;

		SoLoud::handle WinSound = 0;
		SoLoud::handle DefeatSound = 0;
		SoLoud::handle GuitarSound = 0;
		SoLoud::handle DrumSound = 0;
		SoLoud::handle LaunchpadSound = 0;

		int playerFieldsNumber = 0;
		int enemyFieldsNumber = 0;
		AudioComponent* audioComp = nullptr;
		void SetupButtons();
		//void PerformTurn();

		std::string colors[7] = { "blue", "red", "green", "purple", "yellow", "cyan", "pink" };


	public:
		Text* PlayerScore = nullptr;
		Text* EnemyScore = nullptr;
		GameObject* MinigamePlain = nullptr;
		GameObject* WonPanel = nullptr;
		GameObject* LostPanel = nullptr;
		Image* PlayerImage = nullptr;
		Image* PlayerFrontImage = nullptr;
		Image* EnemyImage = nullptr;
		Image* EnemyFrontImage = nullptr;
		Image* PlayerSelectionImage = nullptr;
		Image* EnemySelectionImage = nullptr;

		Button* LaunchpadButton = nullptr;
		Button* GuitarButton = nullptr;
		Button* DrumButton = nullptr;

		size_t MaxNumberOfTurns = 5;


		static MinigameManager* GetLastInstance();

		void StartMinigame(Playable* chalanging, Playable* chalanged);
		void PlayerWon();
		void PlayerDrawed();
		void PlayerLost();
		void FinishMinigame(Playable* winner, Playable* looser);

		virtual void Initialize() override;
		virtual void OnDestroy() override;
		virtual void Update() override;

		virtual void OnEnable() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
		virtual void DrawEditor() override;
#endif
};