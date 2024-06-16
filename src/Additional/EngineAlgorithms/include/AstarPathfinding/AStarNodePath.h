#pragma once

#include <AstarPathfinding/AStarPathfindingNode.h>

namespace AStar
{
	class AStarPathfindingNode;

	class AStarNodePath
	{
		short _current = 0;
		std::vector<AStarPathfindingNode*> _pathNodes;

	public:
		AStarNodePath();
		AStarNodePath(const std::vector<AStarPathfindingNode*>& pathNodes);
		
		unsigned Current() const;
		unsigned Count() const;

		inline bool IsOnEnd() const;
		inline bool IsOnBegin() const;

		AStarPathfindingNode* GetNext() const;
		AStarPathfindingNode* GetCurrent() const;
		AStarPathfindingNode* GetPrevious() const;

		AStarPathfindingNode* Next();
		AStarPathfindingNode* Previous();

		AStarNodePath& operator=(const AStarNodePath& other);
	};
}