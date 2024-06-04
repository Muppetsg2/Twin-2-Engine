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

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Processes;
using namespace Twin2Engine::UI;

class MinigameManager : public Component {
	private:
		static MinigameManager* lastInstance;
		enum Choice {NONE = 0, ROCK = 1, PAPER = 2, SCISSOR = 3};
		Choice playerChoice = Choice::NONE;
		Choice enemyChoice = Choice::NONE;
		int playerWins = 0;
		int enemyWins = 0;

		Player* player = nullptr;
		Enemy* enemy = nullptr;

		Coroutine* waitCoroutine;

		int playerFieldsNumber = 3;
		int enemyFieldsNumber = 3;
		void SetupButtons();
		void PerformTurn();


	public:
		Text* PlayerScore = nullptr;
		Text* EnemyScore = nullptr;
		GameObject* MinigamePlain = nullptr;
		Image* PlayerImage = nullptr;
		Image* EnemyImage = nullptr;
		Image* PlayerSelectionImage = nullptr;
		Image* EnemySelectionImage = nullptr;

		Button* RockButton = nullptr;
		Button* PapperButton = nullptr;
		Button* ScissorButton = nullptr;

		int MaxNumberOfTurns = 5;


		static MinigameManager* GetLastInstance();

		void StartMinigame(Playable* chalanging, Playable* chalanged);
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