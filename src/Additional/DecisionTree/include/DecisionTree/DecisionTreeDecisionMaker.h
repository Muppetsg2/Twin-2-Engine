#pragma once

#include <DecisionTree/DecisionTreeNode.h>
#include <tools/EventHandler.h>

template<class _Entity, class _Type>
class DecisionTreeDecisionMaker : public DecisionTreeNode<_Entity> {
private:
	using DecisionFunc = Twin2Engine::Tools::Func<_Type, _Entity*>;

	DecisionFunc _decision;
	std::unordered_map<_Type, DecisionTreeNode<_Entity>*> _nodes;

public:
	DecisionTreeDecisionMaker(const DecisionFunc& decision, const std::unordered_map<_Type, DecisionTreeNode<_Entity>*>& nodes);

	void ProcessNode(_Entity* entity);
};

template<class _Entity, class _Type>
inline DecisionTreeDecisionMaker<_Entity, _Type>::DecisionTreeDecisionMaker(const DecisionFunc& decision, const std::unordered_map<_Type, DecisionTreeNode<_Entity>*>& nodes)
	: _decision(decision), _nodes(nodes) {}

template<class _Entity, class _Type>
inline void DecisionTreeDecisionMaker<_Entity, _Type>::ProcessNode(_Entity* entity)
{
	auto nodesIter = _nodes.find(_decision(entity));
	if (nodesIter != _nodes.end()) {
		(*nodesIter).second->ProcessNode(entity);
	}
}