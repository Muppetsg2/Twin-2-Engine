#include <Enemy/MovingState.h>
#include <Enemy.h>
#include <EnemyMovement.h>
#include <Abilities/ConcertAbilityController.h>

// TODO: dodaæ na wykryciu radioStation moveToRadioStation (usprawnia kierunek poruszania i daje powód)
DecisionTree<Enemy*, bool> MovingState::_decisionTree{
	[&](Enemy* enemy) -> bool {
		// money >= albumPrice && GlobalAvg(Enemy) <= 50% && albumCooldown <= 0 && !albumActive
		// TODO: Get Current Money
		float currMoney = 0.f;
		return currMoney >= enemy->albumRequiredMoney && enemy->GlobalAvg() <= 50.f && enemy->albumCooldown <= 0.f && !enemy->isAlbumActive;
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
					// TODO: CurrentMoney
					// TODO: ConcertData
					float currMoney = 0.f;
					float concertRequiredMoney = 1.f;
					bool concertIsActive = true;
					float concertCooldown = 1.f;
					return currMoney >= concertRequiredMoney && !concertIsActive && concertCooldown <= 0.f;
				},
				{
					{
						true,
						new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
							ConcertAbility(enemy);
						})
					},
					{
						false,
						new DecisionTreeDecisionMaker<Enemy*, bool>(
							[&](Enemy* enemy) -> bool {
								// LocalAvg(Enemy) >= 50% && LocalTilesTaken(Enemy) == 6 && !PlayerInMoveRange && !RadioStationInRange
								// TODO: Local Taken Tiles
								// TODO: Player In Move Range Analisis
								// TODO: RadioStation In Move Range Analisis
								size_t localTakenTiles = 0;
								bool playerInMoveRange = false;
								bool radioStationInMoveRange = false;
								return enemy->LocalAvg() >= 50.f && localTakenTiles == 6 && !playerInMoveRange && !radioStationInMoveRange;
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
											// TODO: Player In Move Range Analisis
											// TODO: Get If Player is on Enemy Tile
											// TODO: Get Player Tiles Number
											// TODO: Analisis of Enemy And Player Power
											bool playerInMoveRange = false;
											bool playerOnEnemyTile = false;
											size_t playerTiles = 2;
											bool enemyIsStrongerThanPlayer = false;
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
														// Sprawdzenie do kogo nale¿y CurrTile
														// ((!EnemyTile && (!PlayerTile || (PlayerTile && Percent <= 50))) || (EnemyTile && Percent <= 50)) || (RadioStation && RadioCooldown <= 0)
														// TODO: Check if enemy tile
														// TODO: Check if player tile
														// TODO: Check if radioStation tile
														bool enemyTile = false;
														bool playerTile = false;
														float playerTilePercent = 60;
														float enemyTilePercent = 60;
														bool radioStation = false;
														float radioStationCooldown = 1.f;
														return ((!enemyTile && (!playerTile || (playerTile && playerTilePercent <= 50.f)))) || (enemyTile && enemyTilePercent <= 50.f) || (radioStation && radioStationCooldown <= 0.f);
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

	// TODO: Find tile with desired player
	// TODO: Move to tile with desired player

	enemy->ChangeState(&enemy->_fightingState);
}

void MovingState::RadioStation(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Radio Station");

	// TODO: Find tile with radio station
	// TODO: Move to tile with radio station

	enemy->ChangeState(&enemy->_radioStationState);
}

void MovingState::AlbumAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Album Ability");

	// TODO: Use Album Ability
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
	
	size_t ofpeId = (enemy->_movement->OnFindPathError += [&](GameObject* gameObject, HexTile* tile) {
		ChooseTile(enemy);
	});
	size_t ofmId = (enemy->_movement->OnFinishMoving += [&](GameObject* gameObject, HexTile* tile) {
		enemy->FinishedMovement(tile);
		_decisionTree.ProcessNode(enemy);
	});

	_eventsIds[enemy] = { ofpeId, ofmId };
}

void MovingState::Update(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Update Moving State");
	_decisionTree.ProcessNode(enemy);
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