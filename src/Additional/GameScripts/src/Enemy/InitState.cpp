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

HexTile* InitState::ChooseTile(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Init State Choose Tile");

	std::srand(std::time(NULL));
	vector<HexTile*> tiles = GameManager::instance->Tiles;
	// Zmniejsz liczbê mo¿liwych tile-i
	for (size_t i = 0, deleted = 0; i < tiles.size();) {
		HexTile* tile = tiles[i];
		if (tile->occupyingEntity != nullptr || tile->GetMapHexTile()->type != MapHexTile::HexTileType::PointOfInterest) {
			tiles.erase(tiles.begin() + i);
		}
		else {
			++i;
		}
	}
	size_t idx = std::rand() % tiles.size();
	HexTile* tile = tiles[idx];

	enemy->GetTransform()->SetGlobalPosition(tile->GetTransform()->GetGlobalPosition());
	enemy->SetCurrTile(tile);
	
	tile->StartTakingOver(enemy);

	return tile;
}

void InitState::Begin(Enemy* enemy) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Init State Begin");
	
	HexTile* tile = ChooseTile(enemy);
	enemy->GetGameObject()->GetComponent<MeshRenderer>()->SetEnable(true);
	if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation) {
		enemy->ChangeState(&enemy->_radioStationState);
	}
	else {
		enemy->ChangeState(&enemy->_takingOverState);
	}
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