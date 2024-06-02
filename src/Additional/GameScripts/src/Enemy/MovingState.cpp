#include <Enemy/MovingState.h>
#include <Enemy.h>

DecisionTree<Enemy*, bool> MovingState::_decisionTree{
	[&](Enemy* enemy) -> bool {
		// money >= albumPrice && GlobalAvg(Enemy) <= 50% && albumCooldown <= 0 && !albumActive
		return false;
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
					return false;
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
								return false;
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
											return false;
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
														return true;
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