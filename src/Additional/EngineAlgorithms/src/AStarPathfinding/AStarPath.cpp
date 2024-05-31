#include <AstarPathfinding/AStarPath.h>

using namespace AStar;
using namespace glm;
using namespace std;

AStarPath::AStarPath()
{

}

AStarPath::AStarPath(const vector<vec3>& pathNodes)
{
	_pathNodes = pathNodes;
	_current = 0;
}

inline bool AStarPath::IsOnEnd() const
{
	return _current == _pathNodes.size() - 1ull;
}

inline bool AStarPath::IsOnBegin() const
{
	return _current == 0;
}

unsigned AStarPath::Current() const
{
	return _current;
}

unsigned AStarPath::Count() const
{
	return _pathNodes.size();
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

	//++_current;
	//return _pathNodes[_current - 1];
	return _pathNodes[_current++];
}

vec3 AStarPath::Previous()
{
	if (IsOnBegin())
	{
		return _pathNodes[_current];
	}

	//--_current;
	//return _pathNodes[_current + 1];
	return _pathNodes[_current--];
}

AStarPath& AStarPath::operator=(const AStarPath& other)
{
	_pathNodes = other._pathNodes;
	_current = other._current;

	return *this;
}