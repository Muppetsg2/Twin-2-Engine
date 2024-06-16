#include <AstarPathfinding/AStarNodePath.h>

using namespace AStar;
using namespace glm;
using namespace std;

AStarNodePath::AStarNodePath()
{

}

AStarNodePath::AStarNodePath(const vector<AStarPathfindingNode*>& pathNodes)
{
	_pathNodes = pathNodes;
	_current = 0;
}

inline bool AStarNodePath::IsOnEnd() const
{
	return _current == _pathNodes.size() - 1ull;
}

inline bool AStarNodePath::IsOnBegin() const
{
	return _current == 0;
}

unsigned AStarNodePath::Current() const
{
	return _current;
}

unsigned AStarNodePath::Count() const
{
	return _pathNodes.size();
}

AStarPathfindingNode* AStarNodePath::GetNext() const
{
	if (IsOnEnd())
	{
		return _pathNodes[_current];
	}

	return _pathNodes[_current + 1];
}

AStarPathfindingNode* AStarNodePath::GetCurrent() const
{
	return _pathNodes[_current];
}

AStarPathfindingNode* AStarNodePath::GetPrevious() const
{
	if (IsOnBegin())
	{
		return _pathNodes[_current];
	}

	return _pathNodes[_current - 1];
}

AStarPathfindingNode* AStarNodePath::Next()
{
	if (IsOnEnd())
	{
		return _pathNodes[_current];
	}

	//++_current;
	//return _pathNodes[_current - 1];
	return _pathNodes[_current++];
}

AStarPathfindingNode* AStarNodePath::Previous()
{
	if (IsOnBegin())
	{
		return _pathNodes[_current];
	}

	//--_current;
	//return _pathNodes[_current + 1];
	return _pathNodes[_current--];
}

AStarNodePath& AStarNodePath::operator=(const AStarNodePath& other)
{
	_pathNodes = other._pathNodes;
	_current = other._current;

	return *this;
}