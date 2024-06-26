#include <Enemy/MovingState.h>
#include <Enemy.h>
#include <EnemyMovement.h>
#include <Abilities/ConcertAbilityController.h>
#include <RadioStation/RadioStation.h>

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
		if (data.RadioStationTile != nullptr) {
			RadioStation* radioStation = data.RadioStationTile->GetGameObject()->GetComponentInChildren<RadioStation>();
			if (radioStation->GetRemainingCooldownTime() <= 0.f) {
				float dist = INFINITE;
				for (auto& takenTile : data.enemy->OwnTiles) {
					float tempDist = glm::distance((glm::vec2)data.RadioStationTile->GetMapHexTile()->tile->GetPosition(), (glm::vec2)takenTile->GetMapHexTile()->tile->GetPosition());
					if (tempDist < dist) {
						dist = tempDist;
					}
				}
				return dist <= data.enemy->_tilemap->GetDistanceBetweenTiles() * 2;
			}
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
				MoveToRadioStation(data.enemy, data.RadioStationTile);
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
							MoveToFight(data.enemy, data.playerTile);
						})
					},
					{
						false,
						new DecisionTreeDecisionMaker<AfterMoveDecisionData&, bool>(
							[&](AfterMoveDecisionData& data) -> bool {
								// Sprawdzenie do kogo nale�y CurrTile
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

std::unordered_map<Enemy*, bool> MovingState::_afterMove;

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
		if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation) {
			RadioStation* radioStation = tile->GetGameObject()->GetComponentInChildren<RadioStation>();
			if (radioStation->GetRemainingCooldownTime() <= 0.f) {
				data.RadioStationTile = tile;
				break;
			}
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

void MovingState::MoveToFight(Enemy* enemy, HexTile* playerTile)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fight");

	// Move to tile with desired player
	enemy->SetMoveDestination(playerTile);
}

void MovingState::MoveToRadioStation(Enemy* enemy, HexTile* radioStationTile)
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

	// priority
	// 1. is last Visited?
	// 2. is his tiles?
	// 3. distance from own tiles
	// 4. distance from him
	// 5. taken percent
	map<bool, map<bool, map<float, map<float, map<float, vector<HexTile*>>>>>> possible;

	size_t size = GameManager::instance->Tiles.size();
	float maxRadius = enemy->GetMaxRadius();

	for (size_t index = 0ull; index < size; ++index)
	{
		HexTile* tile = GameManager::instance->Tiles[index];
		MapHexTile::HexTileType type = tile->GetMapHexTile()->type;
		if (type != MapHexTile::HexTileType::Mountain && tile != enemy->CurrTile)
		{
			tilePosition = tile->GetTransform()->GetGlobalPosition();
			tilePosition.y = .0f;
			float distance = glm::distance(globalPosition, tilePosition);
			
			if (distance <= maxRadius)
			{
				bool isLastVisited = false;
				for (size_t i = 0; i < enemy->_lastVisitedTiles.size(); ++i) {
					if (tile == enemy->_lastVisitedTiles[i]) {
						isLastVisited = true;
						break;
					}
				}

				float dist = INFINITE;
				for (auto& takenTile : enemy->OwnTiles) {
					vec3 takenTilePosition = takenTile->GetTransform()->GetGlobalPosition();
					takenTilePosition.y = 0.f;
					float tempDist = glm::distance(tilePosition, takenTilePosition);
					if (tempDist < dist) {
						dist = tempDist;
					}
				}

				bool isEnemyOwner = tile->ownerEntity == enemy;
				possible[isLastVisited][isEnemyOwner][dist][distance][tile->percentage].push_back(tile);
			}
		}
	}



	if (possible.size() != 0) {
		auto& tiles = possible.begin()->second.begin()->second.begin()->second.begin()->second.begin()->second;
		HexTile* result = tiles[Random::Range(0ull, tiles.size() - 1ull)];

		enemy->SetCurrTile(nullptr);
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
	
	size_t ofpeId = (enemy->_movement->OnFindPathError += [enemy](GameObject* gameObject, HexTile* tile) {
		ChooseTile(enemy);
	});
	size_t ofmId = (enemy->_movement->OnFinishMoving += [enemy](GameObject* gameObject, HexTile* tile) {
		if (tile == nullptr) {
			SPDLOG_ERROR("TILE WAS NULLPTR. WHY????!!!!");
			ChooseTile(enemy);
		}
		else {
			enemy->SetCurrTile(tile);
			if (tile->occupyingEntity != nullptr && tile->occupyingEntity != enemy && tile->state == TileState::OCCUPIED) {
				tile->StartTakingOver(enemy);
				enemy->ChangeState(&enemy->_fightingState);
			}
			else if (tile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation) {
				RadioStation* radioStation = tile->GetGameObject()->GetComponentInChildren<RadioStation>();
				if (radioStation->GetRemainingCooldownTime() <= 0.f) {
					tile->StartTakingOver(enemy);
					enemy->ChangeState(&enemy->_radioStationState);
				}
				else {
					tile->StartTakingOver(enemy);
				}
			}
			else {
				tile->StartTakingOver(enemy);
			}
			_afterMove[enemy] = true;
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
	if (!_afterMove.contains(enemy)) {
		_whileMovingDecisionTree.ProcessNode(enemy);
	}
	else {
		DoAfterMoveDecisionTree(enemy);
		_afterMove.erase(enemy);
	}
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
	_afterMove.erase(enemy);
}