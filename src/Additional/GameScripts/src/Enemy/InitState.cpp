#include <Enemy/InitState.h>
#include <Enemy.h>

DecisionTree<Enemy*, bool> InitState::_decisionTree{
	[&](Enemy* enemy) -> bool {
		return GameManager::instance->gameStarted;
	},
	{
		{
			true,
			new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
				Begin(enemy);
			})
		}
	}
};

void InitState::ChooseTile(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Init State Choose Tile");

	std::srand(std::time(NULL));
	size_t idx = std::rand() % enemy->_tiles.size();
	HexTile* tile = enemy->_tiles[idx];
	while (tile->occupyingEntity != nullptr) {
		idx = std::rand() % enemy->_tiles.size();
		tile = enemy->_tiles[idx];
	}
	enemy->GetTransform()->SetGlobalPosition(tile->GetTransform()->GetGlobalPosition() + glm::vec3(0.0f, 0.1f, 0.0f));
	enemy->FinishedMovement(tile);
}

void InitState::Begin(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Init State Begin");
	
	ChooseTile(enemy);
	enemy->GetGameObject()->GetComponent<MeshRenderer>()->SetEnable(true);
	enemy->ChangeState(&enemy->_takingOverState);
}

void InitState::Enter(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Init State Enter");
	enemy->GetGameObject()->GetComponent<MeshRenderer>()->SetEnable(false);
}

void InitState::Update(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Init State Update");
	_decisionTree.ProcessNode(enemy);
}

void InitState::Exit(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Init State Exit");
}