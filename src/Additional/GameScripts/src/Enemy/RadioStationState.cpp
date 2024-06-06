#include <Enemy/RadioStationState.h>
#include <Enemy.h>

#include <RadioStation/RadioStation.h>

// TODO: Send Radio Station Data
DecisionTree<std::pair<Enemy*, uint32_t>, bool> RadioStationState::_decisionTree{
	[&](std::pair<Enemy*, uint32_t> data) -> bool {
		std::srand(std::time(NULL));
		return (std::rand() % 101) <= data.first->_noteLuck;
	},
	{
		{
			true,
			new DecisionTreeLeaf<std::pair<Enemy*, uint32_t>>([&](std::pair<Enemy*, uint32_t> data) -> void {
				++data.second;
				if (data.second < 4) _decisionTree.ProcessNode(data);
				else Score(data.first, data.second);
			})
		},
		{
			false,
			new DecisionTreeLeaf<std::pair<Enemy*, uint32_t>>([&](std::pair<Enemy*, uint32_t> data) -> void {
				Score(data.first, data.second);
			})
		}
	}
};

void RadioStationState::Score(Enemy* enemy, uint32_t score)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Radio Station Score {0}", score);

	if (enemy->CurrTile->GetGameObject()->GetComponentInChildren<RadioStation>())
	{
		enemy->CurrTile->GetGameObject()->GetComponentInChildren<RadioStation>()->StartTakingOver(enemy, score / 4.0f);
	}

	enemy->ChangeState(&enemy->_takingOverState);
}

void RadioStationState::Enter(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Enter Radio Station State");

	_decisionTree.ProcessNode({ enemy, 0 });
}

void RadioStationState::Update(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Update Radio Station State");
}

void RadioStationState::Exit(Enemy* enemy)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	SPDLOG_INFO("Exit Radio Station State");
}