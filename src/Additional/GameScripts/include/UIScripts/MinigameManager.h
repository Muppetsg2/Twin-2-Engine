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

class MinigameManager : public Component {
	private:
		static MinigameManager* _instance;

		int _playerWins = 0;
		int _enemyWins = 0;

		Player* _player = nullptr;
		Enemy* _enemy = nullptr;

		Coroutine* _waitCoroutine;

		size_t _winSound = 0;
		size_t _defeatSound = 0;
		size_t _launchpadSound = 0;
		size_t _guitarSound = 0;
		size_t _drumSound = 0;

		int _playerFieldsNumber = 0;
		int _enemyFieldsNumber = 0;
		AudioComponent* _audioComp = nullptr;
		void SetupButtons();

		std::string _colors[7] = { "Blue", "Red", "Green", "Purple", "Yellow", "Cyan", "Pink" };

		Text* _playerScore = nullptr;
		Text* _enemyScore = nullptr;

		GameObject* _minigameCanvas = nullptr;
		GameObject* _wonPanel = nullptr;
		GameObject* _lostPanel = nullptr;

		GameObject* _arrowLD = nullptr;
		GameObject* _arrowDG = nullptr;
		GameObject* _arrowGL = nullptr;

		Image* _playerImage = nullptr;
		Image* _enemyImage = nullptr;

		Button* _launchpadButton = nullptr;
		Button* _guitarButton = nullptr;
		Button* _drumButton = nullptr;

		int _launchpadEvent = -1;
		int _guitarEvent = -1;
		int _drumEvent = -1;

		size_t _minNumberOfWins = 2;
	public:
		static MinigameManager* GetInstance();

		void LaunchpadButtonOnClick();
		void GuitarButtonOnClick();
		void DrumButtonOnClick();

		void StartMinigame(Playable* chalanging, Playable* chalanged);
		void FinishMinigame(Playable* winner, Playable* looser);

		void PlayerWon();
		void PlayerDrawed();
		void PlayerLost();

		virtual void Initialize() override;
		virtual void OnDestroy() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
		virtual void DrawEditor() override;
#endif
};