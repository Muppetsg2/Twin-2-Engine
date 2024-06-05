#include <Enemy/FightingState.h>
#include <Enemy.h>

// TODO: Get Player Choice
DecisionTree<Enemy*, FightingState::FightResult> FightingState::_decisionTree{
	[&](Enemy* enemy) -> FightingState::FightResult {

		// TODO: Sprawdzenie czy gracz cos wybral
		return FightResult::WAIT;

		float score = Score(enemy, enemy->fightingPlayable);
		if (0 <= score && score <= enemy->_winChance) {
			// 0 <= score <= winChance
			return FightResult::WIN;
		}
		else if (enemy->_winChance < score && score <= enemy->_winChance + enemy->_drawChance) {
			// winChance < score <= winChance + drawChance
			return FightResult::DRAW;
		}
		else {
			// winChance + drawChance < score <= 100
			return FightResult::LOSE;
		}
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