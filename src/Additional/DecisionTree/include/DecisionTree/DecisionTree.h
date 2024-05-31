#pragma once

#include <DecisionTree/DecisionTreeDecisionMaker.h>
#include <DecisionTree/DecisionTreeLeaf.h>

template<class _Entity, class _Type> using DecisionTree = DecisionTreeDecisionMaker<_Entity, _Type>;