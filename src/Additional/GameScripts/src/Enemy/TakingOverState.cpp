#include <Enemy/TakingOverState.h>
#include <Enemy.h>
#include <AreaTaking/GetMoneyFromTiles.h>

DecisionTree<Enemy*, bool> TakingOverState::_decisionTree{
	[&](Enemy* enemy) -> bool {
		// money >= FansPrice && !FansActive && FansCooldown <= 0 && FansRangeAvg(Enemy) <= 30%
		return enemy->GetGameObject()->GetComponent<MoneyGainFromTiles>()->money >= enemy->fansRequiredMoney 
			&& enemy->isFansActive && enemy->fansCooldown <= 0.f && enemy->FansRangeAvg() <= 30.f;
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
					// money >= AlbumPrice && AlbumCooldown <= 0 && !AlbumActive && GlobalAvg(Enemy) <= 50%
					// TODO: Add Check if album is ready to buy as Playable func
					return enemy->GetGameObject()->GetComponent<MoneyGainFromTiles>()->money >= enemy->albumRequiredMoney
						&& enemy->albumCooldown <= 0.f && !enemy->isAlbumActive && enemy->GlobalAvg() <= 50.f;
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
								if (enemy->CurrTile == nullptr) return false;

								// (FansActive && LocalAvg(Enemy) >= 75%) || (!FansActive && CurrTilePercent >= 75%)
								return (enemy->isFansActive && enemy->LocalAvg() >= 75.f) || (!enemy->isFansActive && enemy->CurrTile->percentage >= 75.f);
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
};

void TakingOverState::AlbumAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Album Ability");

	enemy->UseAlbum();
}

void TakingOverState::FansMeetingAbility(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Fans Meeting Ability");

	enemy->UseFans();
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