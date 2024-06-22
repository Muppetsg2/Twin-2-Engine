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

DecisionTree<MovingState::AfterMoveDecisionData&, bool> MovingState::_afterMoveDecisionTree{
	[&](AfterMoveDecisionData& data) -> bool {
		// RadioStationInMoveRange && distFromTaken <= 2 * tilesDist
		if (data.RadioStationTile != nullptr && data.RadioStationTile->radioStationCooldown <= 0.f) {
			float dist = INFINITE;
			for (auto& takenTile : data.enemy->OwnTiles) {
				float tempDist = glm::distance((glm::vec2)data.RadioStationTile->GetMapHexTile()->tile->GetPosition(), (glm::vec2)takenTile->GetMapHexTile()->tile->GetPosition());
				if (tempDist < dist) {
					dist = tempDist;
				}
			}
			return dist <= data.enemy->_tilemap->GetDistanceBetweenTiles() * 2;
		}
		return false;

		// LocalAvg(Enemy) >= 50% && LocalTilesTaken(Enemy) == 6 && !PlayerInMoveRange && !RadioStationInRange
		//return data.localAvg >= 50.f && data.LocalTakenTiles.size() == 6 && data.playerTile == nullptr && data.RadioStationTile == nullptr;
	},
	{
		{
			true,
			new DecisionTreeLeaf<AfterMoveDecisionData&>([&](AfterMoveDecisionData& data) -> void {
				// IDZIE DO RADIO STACJI
				RadioStation(data.enemy, data.RadioStationTile);
			})
		},
		{
			false,
			new DecisionTreeDecisionMaker<AfterMoveDecisionData&, bool>(
				[&](AfterMoveDecisionData& data) -> bool {
					// PlayerInMoveRange 
					// && (PlayerOnEnemyTile || PlayerTiles == 1) && EnemyIsStrongerThanPlayer && distToTile <= 2 * distBetweenTiles
					if (data.playerTile != nullptr) {
						float dist = INFINITE;
						for (auto& takenTile : data.enemy->OwnTiles) {
							float tempDist = glm::distance((glm::vec2)data.playerTile->GetMapHexTile()->tile->GetPosition(), (glm::vec2)takenTile->GetMapHexTile()->tile->GetPosition());
							if (tempDist < dist) {
								dist = tempDist;
							}
						}
						return (data.playerOnEnemyTile || data.playerTile->occupyingEntity->OwnTiles.size()) && data.enemyIsStrongerThanPlayer && dist <= data.enemy->_tilemap->GetDistanceBetweenTiles() * 2;
					}
					return false;
				},
				{
					{
						true,
						new DecisionTreeLeaf<AfterMoveDecisionData&>([&](AfterMoveDecisionData& data) -> void {
							// IDZIE DO WALKI
							Fight(data.enemy, data.playerTile);
						})
					},
					{
						false,
						new DecisionTreeDecisionMaker<AfterMoveDecisionData&, bool>(
							[&](AfterMoveDecisionData& data) -> bool {
								// Sprawdzenie do kogo nale¿y CurrTile
								// (!EnemyTile && (!PlayerTile || (PlayerTile && Percent <= 50)) || (EnemyTile && Percent <= 50)
								if (data.enemy->CurrTile->ownerEntity == data.enemy) {
									return data.enemy->CurrTile->percentage <= 50.f;
								}
								else if (data.enemy->CurrTile->ownerEntity != nullptr) {
									return data.enemy->CurrTile->percentage <= 50.f;
								}
								else {
									return true;
								}
							},
							{
								{
									true,
									new DecisionTreeLeaf<AfterMoveDecisionData&>([&](AfterMoveDecisionData& data) -> void {
										// Zostaje na polu i je przejmuje
										StartTakingOver(data.enemy);
									})
								},
								{
									false,
									new DecisionTreeLeaf<AfterMoveDecisionData&>([&](AfterMoveDecisionData& data) -> void {
										// idzie na inne dowolne pole
										Move(data.enemy);
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

void MovingState::DoAfterMoveDecisionTree(Enemy* enemy) {
	AfterMoveDecisionData data = {
		.enemy = enemy,
		.InMoveRangeTiles = enemy->GetInMoveRangeTiles(),
		.LocalTiles = enemy->GetLocalTiles(),
		.LocalTakenTiles = enemy->GetLocalTakenTiles(),
		.localAvg = enemy->LocalAvg()
	};

	data.playerTile = nullptr;
	data.enemyIsStrongerThanPlayer = false;
	data.RadioStationTile = nullptr;
	for (auto& tile : data.InMoveRangeTiles) {
		// Fight
		if (tile->occupyingEntity != nullptr && tile->occupyingEntity != enemy) {
			if (data.playerTile == nullptr) {
				data.playerTile = tile;
				data.playerOnEnemyTile = tile->ownerEntity == enemy;
			}
			else {
				float score = tile->occupyingEntity->FightPowerScore() - data.playerTile->occupyingEntity->FightPowerScore();
				if (score > 0.f) {
					data.enemyIsStrongerThanPlayer = true;
					data.playerTile = tile;
					data.playerOnEnemyTile = tile->ownerEntity == enemy;
				}
				else if (score == 0.f) {
					if (tile->ownerEntity == enemy || data.playerTile->occupyingEntity->OwnTiles.size() > tile->occupyingEntity->OwnTiles.size()) {
						data.enemyIsStrongerThanPlayer = true;
						data.playerTile = tile;
						data.playerOnEnemyTile = tile->ownerEntity == enemy;
					}
				}
			}
		}

		// Radio Station
		if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation && tile->radioStationCooldown <= 0.f) {
			data.RadioStationTile = tile;
			break;
		}
	}

	_afterMoveDecisionTree.ProcessNode(data);
}

void MovingState::StartTakingOver(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Start Taking Over");
	enemy->ChangeState(&enemy->_takingOverState);
}

void MovingState::Fight(Enemy* enemy, HexTile* playerTile)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fight");

	// Move to tile with desired player
	enemy->SetMoveDestination(playerTile);
}

void MovingState::RadioStation(Enemy* enemy, HexTile* radioStationTile)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Radio Station");

	// Move to tile with desired radioStation
	enemy->SetMoveDestination(radioStationTile);
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

void MovingState::ChooseTile(Enemy* enemy)
{
	vec3 globalPosition = enemy->GetTransform()->GetGlobalPosition();
	globalPosition.y = 0.0f;

	vec3 tilePosition;

	// priority (distance from own tiles)
	map<float, vector<HexTile*>> possible;

	size_t size = GameManager::instance->Tiles.size();
	float maxRadius = enemy->GetMaxRadius();

	for (size_t index = 0ull; index < size; ++index)
	{
		MapHexTile::HexTileType type = GameManager::instance->Tiles[index]->GetMapHexTile()->type;
		if (type != MapHexTile::HexTileType::Mountain && type != MapHexTile::HexTileType::None && GameManager::instance->Tiles[index] != enemy->CurrTile)
		{
			tilePosition = GameManager::instance->Tiles[index]->GetTransform()->GetGlobalPosition();
			tilePosition.y = .0f;
			float distance = glm::distance(globalPosition, tilePosition);
			
			float dist = INFINITE;
			for (auto& takenTile : enemy->OwnTiles) {
				float tempDist = glm::distance((glm::vec2)GameManager::instance->Tiles[index]->GetMapHexTile()->tile->GetPosition(), (glm::vec2)takenTile->GetMapHexTile()->tile->GetPosition());
				if (tempDist < dist) {
					dist = tempDist;
				}
			}
			
			if (distance <= maxRadius)
			{
				if (!possible.contains(dist)) possible[dist] = vector<HexTile*>();
				possible[dist].push_back(GameManager::instance->Tiles[index]);
			}
		}
	}

	SPDLOG_INFO("ENEMY Possible Size: {}", possible.size());
	if (possible.size() != 0) {
		auto& possiblePair = *possible.begin();
		HexTile* result = possiblePair.second[Random::Range(0ull, possiblePair.second.size() - 1ull)];
		enemy->SetMoveDestination(result);
	}
	else {
		StartTakingOver(enemy);
	}
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
		if (tile == nullptr) {
			SPDLOG_ERROR("TILE WAS NULLPTR. WHY????!!!!");
			ChooseTile(enemy);
		}
		else {
			enemy->CurrTile = tile;

			if (tile->occupyingEntity != nullptr && tile->occupyingEntity != enemy && tile->state == TileState::OCCUPIED) {
				tile->StartTakingOver(enemy);
				enemy->ChangeState(&enemy->_fightingState);
			}
			else if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation) {
				if (tile->radioStationCooldown <= 0.f) {
					tile->StartTakingOver(enemy);
					enemy->ChangeState(&enemy->_radioStationState);
				}
				else {
					ChooseTile(enemy);
				}
			}
			else {
				tile->StartTakingOver(enemy);
				DoAfterMoveDecisionTree(enemy);
			}
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