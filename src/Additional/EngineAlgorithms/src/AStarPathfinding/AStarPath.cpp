#include <AstarPathfinding/AStarPath.h>

using namespace AStar;
using namespace glm;
using namespace std;

AStarPath::AStarPath(const vector<vec3>& pathNodes)
{
	_pathNodes = pathNodes;
}

inline bool AStarPath::IsOnEnd() const
{
	return _current == _pathNodes.size();
}

inline bool AStarPath::IsOnBegin() const
{
	return _current == 0;
}

vec3 AStarPath::GetNext() const
{
	if (IsOnEnd())
	{
		return _pathNodes[_current];
	}

	return _pathNodes[_current + 1];
}

vec3 AStarPath::GetCurrent() const
{
	return _pathNodes[_current];
}

vec3 AStarPath::GetPrevious() const
{
	if (IsOnBegin())
	{
		return _pathNodes[_current];
	}

	return _pathNodes[_current - 1];
}

vec3 AStarPath::Next()
{
	if (IsOnEnd())
	{
		return _pathNodes[_current];
	}

	++_current;
	return _pathNodes[_current];
}

vec3 AStarPath::Previous()
{
	if (IsOnBegin())
	{
		return _pathNodes[_current];
	}

	--_current;
	return _pathNodes[_current];
}
