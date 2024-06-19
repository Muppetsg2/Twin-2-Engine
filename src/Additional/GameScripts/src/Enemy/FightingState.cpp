#include <Enemy/FightingState.h>
#include <Enemy.h>

#include <UIScripts/MinigameManager.h>

// TODO: Get Player Choice
DecisionTree<Enemy*, FightingState::FightResult> FightingState::_decisionTree{
	[&](Enemy* enemy) -> FightingState::FightResult {

		//// TODO: Na razie nie ma fighting system
		//return FightResult::LOSE;

		// Sprawdzenie czy gracz cos wybral
		if (enemy->fightingPlayable->minigameChoice == MinigameRPS_Choice::NONE) {
			return FightResult::WAIT;
		}

		Player* player = dynamic_cast<Player*>(enemy->fightingPlayable);

		float score = Score(enemy, enemy->fightingPlayable);

		if (0 <= score && score <= enemy->_winChance) {
			// 0 <= score <= winChance
			if (player != nullptr) {
				MinigameManager::GetInstance()->PlayerLost();
				if (MinigameManager::GetInstance()->GetNumberOfWinsEnemy() == MinigameManager::GetInstance()->GetMinNumberOfWins()) return FightResult::WIN;
				else return FightResult::WAIT;
			}
			else {
				Lose((Enemy*)enemy->fightingPlayable);
				enemy->WonPaperRockScissors(enemy->fightingPlayable);
				return FightResult::WIN;
			}
		}
		else if (enemy->_winChance < score && score <= enemy->_winChance + enemy->_drawChance) {
			// winChance < score <= winChance + drawChance
			if (player != nullptr) { // rozpatrywanie remisu z graczem
				MinigameManager::GetInstance()->PlayerDrawed();
				return FightResult::DRAW;
			}
			else { // ropatrywanie remisu z innym enemy
				if (score <= (enemy->_winChance + 0.5f * enemy->_drawChance)) {
					Lose((Enemy*)enemy->fightingPlayable);
					enemy->WonPaperRockScissors(enemy->fightingPlayable);
					return FightResult::WIN;
				}
				else {
					Win((Enemy*)enemy->fightingPlayable);
					enemy->fightingPlayable->WonPaperRockScissors(enemy);
					return FightResult::LOSE;
				}
			}
		}
		else {
			// winChance + drawChance < score <= 100
			if (player != nullptr) {
				MinigameManager::GetInstance()->PlayerWon();
				if (MinigameManager::GetInstance()->GetNumberOfWinsPlayer() == MinigameManager::GetInstance()->GetMinNumberOfWins()) return FightResult::LOSE;
				else return FightResult::WAIT;
			}
			else {
				Win((Enemy*)enemy->fightingPlayable);
				enemy->fightingPlayable->WonPaperRockScissors(enemy);
				return FightResult::LOSE;
			}
		}

		/*
		if (0 <= score && score <= enemy->_winChance) {
			// 0 <= score <= winChance
			if (player != nullptr) {
				MinigameManager::GetInstance()->PlayerLost();
			}
			else {
				Lose((Enemy*)enemy->fightingPlayable);
				enemy->WonPaperRockScissors(enemy->fightingPlayable);
			}
			return FightResult::WIN;
		}
		else if (enemy->_winChance < score && score <= enemy->_winChance + enemy->_drawChance) {
			// winChance < score <= winChance + drawChance
			if (player != nullptr) { // rozpatrywanie remisu z graczem
				MinigameManager::GetInstance()->PlayerDrawed();
				return FightResult::DRAW;
			}
			else { // ropatrywanie remisu z innym enemy
				if (score <= (enemy->_winChance + 0.5f * enemy->_drawChance)) {
					Lose((Enemy*)enemy->fightingPlayable);
					enemy->WonPaperRockScissors(enemy->fightingPlayable);
					return FightResult::WIN;
				}
				else {
					Win((Enemy*)enemy->fightingPlayable);
					enemy->fightingPlayable->WonPaperRockScissors(enemy);
					return FightResult::LOSE;
				}
			}
		}
		else {
			// winChance + drawChance < score <= 100
			if (player != nullptr) { 
				MinigameManager::GetInstance()->PlayerWon();
			}
			else {
				Win((Enemy*)enemy->fightingPlayable);
				enemy->fightingPlayable->WonPaperRockScissors(enemy);
			}

			return FightResult::LOSE;
		}
		*/
	},
	{
		{ 
			FightResult::WIN,
			new DecisionTreeLeaf<Enemy*>
			(
				[&](Enemy* enemy) -> void {
					Win(enemy);
				}
			)
		},
		{
			FightResult::DRAW,
			new DecisionTreeLeaf<Enemy*>
			(
				[&](Enemy* enemy) -> void {
					Draw(enemy);
				}
			)
		},
		{
			FightResult::LOSE,
			new DecisionTreeLeaf<Enemy*>
			(
				[](Enemy* enemy) -> void {
					Lose(enemy);
				}
			)
		}
	}
};

float FightingState::Score(Enemy* enemy, Playable* entity) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	std::srand(std::time(NULL));
	float rnd = std::rand() % 101;
	// clamp(rand(0, 100) + map(fightPowerScore(player) - fightPowerScore(enemy), -100, 100, -50, 50), 0, 100)
	return std::clamp(rnd + (entity->FightPowerScore() - enemy->FightPowerScore()) * .5f, 0.f, 100.f);
}

void FightingState::Draw(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("ENEMY DRAW");
	// TODO: Dodac analize ruchu gracza
	// TODO: Dodac odpowiedz na ruch gracza
}

void FightingState::Win(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("ENEMY WIN");
	// TODO: Dodac analize ruchu gracza
	// TODO: Dodac odpowiedz na ruch gracza

	if (enemy->CurrTile->GetMapHexTile()->type == Generation::MapHexTile::HexTileType::RadioStation) {
		enemy->ChangeState(&enemy->_radioStationState);
	}
	else {
		enemy->ChangeState(&enemy->_takingOverState);
	}
}

void FightingState::Lose(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("ENEMY LOSE");
	// TODO: Dodac analize ruchu gracza
	// TODO: Dodac odpowiedz na ruch gracza
	enemy->ChangeState(&enemy->_movingState);
}

void FightingState::Enter(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fighting State Enter");
}

void FightingState::Update(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fighting State Update");

	_decisionTree.ProcessNode(enemy);
}

void FightingState::Exit(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fighting State Exit");
}