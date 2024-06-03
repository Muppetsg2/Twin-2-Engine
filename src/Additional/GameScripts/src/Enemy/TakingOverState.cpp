#include <Enemy/TakingOverState.h>
#include <Enemy.h>

DecisionTree<Enemy*, bool> TakingOverState::_decisionTree{
	[&](Enemy* enemy) -> bool {
		// money >= FansPrice && !FansActive && FansCooldown <= 0 && LocalAvg(Enemy) <= 30%
		// TODO: Fans Range Tiles Avg
		// TODO: Get Enemy money
		float currentMoney = 0.f;
		return currentMoney >= enemy->fansRequiredMoney && enemy->isFansActive && enemy->fansCooldown <= 0.f && enemy->LocalAvg() <= 30.f;
	},
	{
		{
			true,
			new DecisionTreeLeaf<Enemy*>([&](Enemy* enemy) -> void {
				FansMeetingAbility(enemy);
			})
		},
		{
			false,
			new DecisionTreeDecisionMaker<Enemy*, bool>(
				[&](Enemy* enemy) -> bool {
					// money >= ConcertPrice && !ConcertActive && ConcertCooldown <= 0 && FansActive
					// TODO: Get Current Money
					// TODO: Concert Data
					float currentMoney = 0.f;
					float concertPrice = 1.f;
					bool isConcertActive = true;
					float concertCooldown = 1.f;
					return currentMoney >= concertPrice && !isConcertActive && concertCooldown <= 0.f && enemy->isFansActive;
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
								// TODO: Get current Money
								// TODO: Add Check if album is ready to buy as Playable func
								float currMoney = 0.f;
								return currMoney >= enemy->albumRequiredMoney && enemy->albumCooldown <= 0.f && !enemy->isAlbumActive && enemy->GlobalAvg() <= 50.f;
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
											// TODO: Get if fans is Active
											bool fansActive = false;
											return (fansActive && enemy->LocalAvg() >= 75.f) || (!fansActive && enemy->CurrTile->percentage >= 75.f);
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