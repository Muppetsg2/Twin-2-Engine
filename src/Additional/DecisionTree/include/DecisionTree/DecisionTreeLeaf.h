#pragma once

#include <DecisionTree/DecisionTreeNode.h>
#include <tools/EventHandler.h>

template<class _Entity>
class DecisionTreeLeaf : public DecisionTreeNode<_Entity> {
private:
	using LeafAction = Twin2Engine::Tools::Action<_Entity>;
	LeafAction _action;

public:
	DecisionTreeLeaf(const LeafAction& action);
	virtual ~DecisionTreeLeaf() = default;

	void ProcessNode(_Entity entity) override;
};

template<class _Entity>
inline DecisionTreeLeaf<_Entity>::DecisionTreeLeaf(const LeafAction& action) : _action(action) {}

template<class _Entity>
inline void DecisionTreeLeaf<_Entity>::ProcessNode(_Entity entity) {
#if TRACY_PROFILER
	ZoneScoped;
#endif

	_action(entity);
}

/*#define DecisionTreeLeaf(_Entity, dataName, body) \
	new DecisionTreeLeaf<_Entity>([&](_Entity dataName) -> void {\
		body\
	})*/