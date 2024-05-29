#include <EnemyAI/FightingState.h>
#include <EnemyAI/Enemy.h>

// DECISION TREE
//   [?]
// F / \ T
//  -   ()

DecisionTree<Enemy, bool> FightingState::_decisionTree{
	[](Enemy* enemy) -> bool {
		return true;
	},
	{
		{ 
			true, 
			new DecisionTreeLeaf<Enemy>
			(
				[](Enemy* enemy) -> void {
					SPDLOG_INFO("True Fighting Decision");
				}
			)
		}
	}
};

void FightingState::Enter(Enemy* enemy)
{
	SPDLOG_INFO("Fighting State Enter");
}

void FightingState::Update(Enemy* enemy)
{
	SPDLOG_INFO("Fighting State Update");
	_decisionTree.ProcessNode(enemy);
}

void FightingState::Exit(Enemy* enemy)
{
	SPDLOG_INFO("Fighting State Exit");
}