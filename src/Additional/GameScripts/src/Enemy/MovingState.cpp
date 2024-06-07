#include <Enemy/MovingState.h>
#include <Enemy.h>
#include <EnemyMovement.h>
#include <Abilities/ConcertAbilityController.h>

DecisionTree<Enemy*, bool> MovingState::_whileMovingDecisionTree{
	[&](Enemy* enemy) -> bool {
		// money >= albumPrice && GlobalAvg(Enemy) <= 50% && albumCooldown <= 0 && !albumActive
		return enemy->GetGameObject()->GetComponent<MoneyGainFromTiles>()->money >= enemy->albumRequiredMoney 
			&& enemy->GlobalAvg() <= 50.f && enemy->albumCooldown <= 0.f && !enemy->isAlbumActive;
	},
	{
		{
			true,
			new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
				AlbumAbility(enemy);
			})
		},
		{
			false,
			new DecisionTreeDecisionMaker<Enemy*, bool>(
				[&](Enemy* enemy) -> bool {
					// money >= concertPrice && !concertActive && concertCooldown <= 0
					// TODO: ConcertData
					ConcertAbilityController* c = enemy->GetGameObject()->GetComponent<ConcertAbilityController>();
					float concertRequiredMoney = 0.f;
					bool concertIsActive = true;
					float concertCooldown = 1.f;
					return enemy->GetGameObject()->GetComponent<MoneyGainFromTiles>()->money >= concertRequiredMoney
						&& !concertIsActive && concertCooldown <= 0.f;
				},
				{
					{
						true,
						new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
							ConcertAbility(enemy);
						})
					}
				}
			)
		}
	}
};

DecisionTree<Enemy*, bool> MovingState::_afterMoveDecisionTree{
	[&](Enemy* enemy) -> bool {
		// LocalAvg(Enemy) >= 50% && LocalTilesTaken(Enemy) == 6 && !PlayerInMoveRange && !RadioStationInRange
		std::vector<HexTile*> inMoveRangeTiles = enemy->GetInMoveRangeTiles();
		bool playerInMoveRange = false;
		bool radioStationInMoveRange = false;
		for (auto& tile : inMoveRangeTiles) {
			if (tile->occupyingEntity != nullptr && tile->occupyingEntity != enemy) {
				playerInMoveRange = true;
			}
			if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation) {
				radioStationInMoveRange = true;
			}
		}
		inMoveRangeTiles.clear();
		return enemy->LocalAvg() >= 50.f && enemy->GetLocalTakenTiles().size() == 6 && !playerInMoveRange && !radioStationInMoveRange;
	},
	{
		{
			true,
			new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
				// IDZIE DO RADIO STACJI
				RadioStation(enemy);
			})
		},
		{
			false,
			new DecisionTreeDecisionMaker<Enemy*, bool>(
				[&](Enemy* enemy) -> bool {
					// PlayerInMoveRange && (PlayerOnEnemyTile || PlayerTiles == 1) && EnemyIsStrongerThanPlayer
					std::vector<HexTile*> inMoveRangeTiles = enemy->GetInMoveRangeTiles();
					bool playerInMoveRange = false;
					bool playerOnEnemyTile = false;
					size_t playerTiles = 0;
					bool enemyIsStrongerThanPlayer = false;
					for (auto& tile : inMoveRangeTiles) {
						if (tile->occupyingEntity != nullptr && tile->occupyingEntity != enemy) {
							if (playerTiles > tile->occupyingEntity->OwnTiles.size() || !playerInMoveRange) {
								playerTiles = tile->occupyingEntity->OwnTiles.size();
							}
							if (tile->takenEntity == enemy) {
								playerOnEnemyTile = true;
							}
							playerInMoveRange = true;

							if (tile->occupyingEntity->FightPowerScore() - enemy->FightPowerScore() < 0.f) {
								enemyIsStrongerThanPlayer = true;
							}
						}
					}
					inMoveRangeTiles.clear();

					return playerInMoveRange && (playerOnEnemyTile || playerTiles == 1) && enemyIsStrongerThanPlayer;
				},
				{
					{
						true,
						new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
							// IDZIE DO WALKI
							Fight(enemy);
						})
					},
					{
						false,
						new DecisionTreeDecisionMaker<Enemy*, bool>(
							[&](Enemy* enemy) -> bool {
								if (enemy->CurrTile == nullptr) {
									return true;
								}

								// Sprawdzenie do kogo nale¿y CurrTile
								// ((!EnemyTile && (!PlayerTile || (PlayerTile && Percent <= 50))) || (EnemyTile && Percent <= 50))
								if (enemy->CurrTile->takenEntity == enemy) {
									return enemy->CurrTile->percentage <= 50.f;
								}
								else if (enemy->CurrTile->takenEntity != nullptr) {
									return enemy->CurrTile->percentage <= 50.f;
								}
								else {
									return true;
								}
							},
							{
								{
									true,
									new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
										// Zostaje na polu i je przejmuje
										StartTakingOver(enemy);
									})
								},
								{
									false,
									new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
										// idzie na inne dowolne pole
										Move(enemy);
									})
								}
							}
						)
					}
				}
			)
		}
	}
};

void MovingState::StartTakingOver(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Start Taking Over");
	enemy->ChangeState(&enemy->_takingOverState);
}

void MovingState::Fight(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fight");

	// Get Desired Tile
	std::vector<HexTile*> inMoveRangeTiles = enemy->GetInMoveRangeTiles();
	HexTile* desiredTile = nullptr;
	for (auto& tile : inMoveRangeTiles) {
		if (tile->occupyingEntity != nullptr && tile->occupyingEntity != enemy) {
			if (desiredTile == nullptr) {
				desiredTile = tile;
			}
			else {
				float score = tile->occupyingEntity->FightPowerScore() - desiredTile->occupyingEntity->FightPowerScore();
				if (score > 0.f) {
					desiredTile = tile;
				}
				else if (score == 0.f) {
					if (tile->takenEntity == enemy || desiredTile->occupyingEntity->OwnTiles.size() > tile->occupyingEntity->OwnTiles.size()) {
						desiredTile = tile;
					}
				}
			}
		}
	}
	inMoveRangeTiles.clear();

	// Move to tile with desired player
	enemy->SetMoveDestination(desiredTile);
}

void MovingState::RadioStation(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Radio Station");

	// Get Desired Tile
	std::vector<HexTile*> inMoveRangeTiles = enemy->GetInMoveRangeTiles();
	HexTile* desiredTile = nullptr;
	for (auto& tile : inMoveRangeTiles) {
		if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation && tile->radioStationCooldown <= 0.f) {
			desiredTile = tile;
			break;
		}
	}
	inMoveRangeTiles.clear();

	// Move to tile with desired radioStation
	enemy->SetMoveDestination(desiredTile);
}

void MovingState::AlbumAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Album Ability");

	// Use Album Ability
	enemy->UseAlbum();
}

void MovingState::ConcertAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Concert Ability");

	// TODO: Use Concert Ability
}

void MovingState::Move(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Move");
	// Move to other tile
	enemy->ChangeState(&enemy->_movingState);
}

// TODO: Zmieniæ ChooseTile
void MovingState::ChooseTile(Enemy* enemy)
{
	vec3 globalPosition = enemy->GetTransform()->GetGlobalPosition();
	globalPosition.y = 0.0f;

	vec3 tilePosition;

	vector<HexTile*> possible;
	//possible.reserve((1 + _movement->maxSteps) / 2 * _movement->maxSteps * 6);
	possible.reserve((1 + enemy->_movement->maxSteps) * enemy->_movement->maxSteps * 3);

	list<HexTile*> tempList = enemy->_tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
	enemy->_tiles.clear();
	enemy->_tiles.insert(enemy->_tiles.begin(), tempList.cbegin(), tempList.cend());

	size_t size = enemy->_tiles.size();
	float maxRadius = enemy->GetMaxRadius();

	for (size_t index = 0ull; index < size; ++index)
	{
		MapHexTile::HexTileType type = enemy->_tiles[index]->GetMapHexTile()->type;
		if (type != MapHexTile::HexTileType::Mountain && type != MapHexTile::HexTileType::None)
		{
			tilePosition = enemy->_tiles[index]->GetTransform()->GetGlobalPosition();
			tilePosition.y = .0f;
			float distance = glm::distance(globalPosition, tilePosition);
			if (distance <= maxRadius)
			{
				possible.push_back(enemy->_tiles[index]);
			}
		}
	}

	SPDLOG_INFO("ENEMY Possible Size: {}", possible.size());
	HexTile* result = possible[Random::Range(0ull, possible.size() - 1ull)];

	enemy->SetMoveDestination(result);
}

std::unordered_map<Enemy*, std::pair<size_t, size_t>> MovingState::_eventsIds;

void MovingState::Enter(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Enter Moving State");

	if (enemy->CurrTile != nullptr) {
		enemy->CurrTile->StopTakingOver(enemy);
	}
	
	size_t ofpeId = (enemy->_movement->OnFindPathError += [enemy](GameObject* gameObject, HexTile* tile) {
		ChooseTile(enemy);
	});
	size_t ofmId = (enemy->_movement->OnFinishMoving += [enemy](GameObject* gameObject, HexTile* tile) {
		if (enemy->CurrTile && enemy->CurrTile != tile)
		{
			enemy->CurrTile->StopTakingOver(enemy);
		}
		enemy->CurrTile = tile;
		tile->StartTakingOver(enemy);

		if (tile->occupyingEntity != nullptr) {
			enemy->ChangeState(&enemy->_fightingState);
		}
		else if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation) {
			enemy->ChangeState(&enemy->_radioStationState);
		}
		else {
			_afterMoveDecisionTree.ProcessNode(enemy);
		}
	});

	_eventsIds[enemy] = { ofpeId, ofmId };

	ChooseTile(enemy);
}

void MovingState::Update(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Update Moving State");
	_whileMovingDecisionTree.ProcessNode(enemy);
}

void MovingState::Exit(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Exit Moving State");
	enemy->_movement->OnFindPathError -= _eventsIds[enemy].first;
	enemy->_movement->OnFinishMoving -= _eventsIds[enemy].second;
	_eventsIds.erase(enemy);
}