#include <EnemyAI/FightingState.h>
#include <EnemyAI/EnemyAI.h>

// DECISION TREE
//   [?]
// F / \ T
//  -   ()

DecisionTree<EnemyAI, bool> FightingState::_decisionTree{
	[](EnemyAI* enemy) -> bool {
		return true;
	},
	{
		{ 
			true, 
			new DecisionTreeLeaf<EnemyAI>
			(
				[](EnemyAI* enemy) -> void {
					SPDLOG_INFO("True Fighting Decision");
				}
			)
		}
	}
};

void FightingState::Enter(EnemyAI* enemy)
{
	SPDLOG_INFO("Fighting State Enter");
}

void FightingState::Update(EnemyAI* enemy)
{
	SPDLOG_INFO("Fighting State Update");
	_decisionTree.ProcessNode(enemy);
}

void FightingState::Exit(EnemyAI* enemy)
{
	SPDLOG_INFO("Fighting State Exit");
}