#pragma once

template<class _Entity>
class DecisionTreeNode {
public:
	DecisionTreeNode() = default;
	virtual ~DecisionTreeNode() = default;
	virtual void ProcessNode(_Entity entity) = 0;
};