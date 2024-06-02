#include <Enemy/TakingOverState.h>
#include <Enemy.h>

/*DecisionTree<Enemy*, bool> TakingOverState::_decisionTree{
	[&](Enemy* enemy) -> bool {
		// money >= FansPrice && !FansActive && FansCooldown <= 0 && LocalAvg(Enemy) <= 30%
		return false;
	},
	{
		{
			true,
			DecisionTreeLeaf(Enemy*, enemy, 
				FansMeetingAbility(enemy);
			)
		},
		{
			false,
			new DecisionTreeDecisionMaker<Enemy*, bool>(
				[&](Enemy* enemy) -> bool {
					// money >= ConcertPrice && !ConcertActive && ConcertCooldown <= 0 && FansActive
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
								// money >= AlbumPrice && AlbumCooldown <= 0 && !AlbumActive && GlobalAvg(Enemy) <= 50%
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
											// (FansActive && LocalAvg(Enemy) >= 75%) || (!FansActive && CurrTilePercent >= 75%)
											return true;
										},
										{
											{
												true,
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
};*/

DecisionTree<Enemy*, bool> TakingOverState::_decisionTree{
	DecisionFunc(Enemy*, bool,
		enemy,
		return false; // money >= FansPrice && !FansActive && FansCooldown <= 0 && LocalAvg(Enemy) <= 30%
	),
	DecisionResults(
		true,
		DecisionTreeLeaf(Enemy*,
			enemy,
			FansMeetingAbility(enemy);
		),

		false,
		DecisionTreeDecisionMaker(Enemy*, bool,
			DecisionFunc(Enemy*, bool,
				enemy,
				return false; // money >= ConcertPrice && !ConcertActive && ConcertCooldown <= 0 && FansActive
			),
			DecisionResults(
				true,
				DecisionTreeLeaf(Enemy*,
					enemy,
					Move(enemy);
				)
			)
		)
	)
};

void TakingOverState::AlbumAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Album Ability");
}

void TakingOverState::ConcertAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Concert Ability");
}

void TakingOverState::FansMeetingAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fans Meeting Ability");
}

void TakingOverState::Move(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Move");
	enemy->ChangeState(&enemy->_movingState);
}

void TakingOverState::Enter(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Enter Taking Over State");
}

void TakingOverState::Update(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Update Taking Over State");
	_decisionTree.ProcessNode(enemy);
}

void TakingOverState::Exit(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Exit Taking Over State");
}