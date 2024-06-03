#include <Enemy/MovingState.h>
#include <Enemy.h>

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
										Move(enemy);
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
													Fight(enemy);
												})
											},
											{
												false,
												new DecisionTreeDecisionMaker<Enemy*, bool>(
													[&](Enemy* enemy) -> bool {
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
																StartTakingOver(enemy);
															})
														},
														{
															false,
															new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
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
	if (enemy->CurrTile->GetMapHexTile()->type == MapHexTile::HexTileType::RadioStation) {
		enemy->ChangeState(&enemy->_radioStationState);
	}
	else {
		enemy->ChangeState(&enemy->_takingOverState);
	}
}

void MovingState::Fight(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fight");
	enemy->ChangeState(&enemy->_fightingState);
}

void MovingState::AlbumAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Album Ability");
}

void MovingState::ConcertAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Concert Ability");
}

void MovingState::Move(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Move");
	enemy->ChangeState(&enemy->_movingState);
}

// TODO: Choose Tile and make decision on Enter
void MovingState::Enter(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Enter Moving State");
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
}