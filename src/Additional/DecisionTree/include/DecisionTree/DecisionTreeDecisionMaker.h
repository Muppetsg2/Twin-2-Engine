#pragma once

#include <DecisionTree/DecisionTreeNode.h>
#include <tools/EventHandler.h>
#include <tools/macros.h>

template<class _Entity, class _Type>
class DecisionTreeDecisionMaker : public DecisionTreeNode<_Entity> {
private:
	using DecisionFunc = Twin2Engine::Tools::Func<_Type, _Entity>;

	DecisionFunc _decision;
	std::unordered_map<_Type, DecisionTreeNode<_Entity>*> _nodes; // Czyœciæ w destruktorze

public:
	DecisionTreeDecisionMaker(const DecisionFunc& decision, const std::unordered_map<_Type, DecisionTreeNode<_Entity>*>& nodes);
	virtual ~DecisionTreeDecisionMaker();

	void ProcessNode(_Entity entity) override;
};

template<class _Entity, class _Type>
inline DecisionTreeDecisionMaker<_Entity, _Type>::DecisionTreeDecisionMaker(const DecisionFunc& decision, const std::unordered_map<_Type, DecisionTreeNode<_Entity>*>& nodes)
	: _decision(decision), _nodes(nodes) 
{
#if TRACY_PROFILER
	ZoneScoped;
#endif
}

template<class _Entity, class _Type>
inline DecisionTreeDecisionMaker<_Entity, _Type>::~DecisionTreeDecisionMaker() {
#if TRACY_PROFILER
	ZoneScoped;
#endif
	for (auto& node : _nodes) {
		delete node.second;
	}
	_nodes.clear();
}

template<class _Entity, class _Type>
inline void DecisionTreeDecisionMaker<_Entity, _Type>::ProcessNode(_Entity entity)
{
#if TRACY_PROFILER
	ZoneScoped;
#endif

	auto nodesIter = _nodes.find(_decision(entity));
	if (nodesIter != _nodes.end()) {
		(*nodesIter).second->ProcessNode(entity);
	}
}

/*#define DecisionFunc(_Entity, _Type, valueName, body)\
	[&](_Entity valueName) -> _Type {\
		body\
	}

#define DecisionResult(value, node) \
	{\
		value,\
		node\
	}

#define DecisionResults(...)\
	{\
		LIST_DO_FOR_EACH_PAIR(DecisionResult, __VA_ARGS__)\
	}

#define DecisionTreeDecisionMaker(_Entity, _Type, decision, results) \
	new DecisionTreeDecisionMaker<_Entity, _Type>(\
		decision,\
		results\
	)*/