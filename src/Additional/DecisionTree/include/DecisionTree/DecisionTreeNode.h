#pragma once

template<class _Entity>
class DecisionTreeNode {
public:
	virtual void ProcessNode(_Entity* entity) = 0;
};