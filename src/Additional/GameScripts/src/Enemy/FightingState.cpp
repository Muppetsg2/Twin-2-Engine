#include <Enemy/FightingState.h>
#include <Enemy.h>

DecisionTree<Enemy*, FightingState::FightResult> FightingState::_decisionTree{
	[&](Enemy* enemy) -> FightingState::FightResult {
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

float FightingState::GlobalAvg(Playable* entity) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	float res = 0.f;
	for (auto& tile : entity->tiles) {
		res += tile->percentage;
	}
	// AllTakenTilesPercent / TakenTilesCount
	return res / entity->tiles.size();
}

float FightingState::LocalAvg(Playable* entity) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	float res = 0.f;
	size_t count = 0;

	vec3 currTilePos = entity->CurrTile->GetTransform()->GetGlobalPosition();
	for (auto& tile : entity->tiles) {
		if (glm::distance(currTilePos, tile->GetTransform()->GetGlobalPosition()) <= entity->GetMaxRadius()) {
			res += tile->percentage;
			++count;
		}
	}
	// AllTakenTilesNextToCurrentTilePercent / TakenTilesNextToCurrentTileCount
	return res / count;
}

float FightingState::SumTiles(Playable* entity) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	// clamp(GlobalAvg(entity) * 0.25 + LocalAvg(entity) * 0.75, 0, 100)
	return std::clamp(GlobalAvg(entity) * 0.25f + LocalAvg(entity) * 0.75f, 0.f, 100.f);
}

float FightingState::Score(Enemy* enemy, Playable* entity) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	std::srand(std::time(NULL));
	float rnd = std::rand() % 101;
	// clamp(rand(0, 100) + map(sumTiles(player) - sumTiles(enemy), -100, 100, -50, 50), 0, 100)
	return std::clamp(rnd + (SumTiles(entity) - SumTiles(enemy)) * .5f, 0.f, 100.f);
}

void FightingState::Draw(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("ENEMY DRAW");
}

void FightingState::Win(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("ENEMY WIN");
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