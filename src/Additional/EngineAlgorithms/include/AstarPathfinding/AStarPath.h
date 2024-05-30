#ifndef _ASTAR_PATH_H_
#define	_ASTAR_PATH_H_

namespace AStar
{
	class AStarPath
	{
		short _current = 0;
		std::vector<glm::vec3> _pathNodes;

	public:
		AStarPath();
		AStarPath(const std::vector<glm::vec3>& pathNodes);
		
		inline unsigned Current() const;
		inline unsigned Count() const;

		inline bool IsOnEnd() const;
		inline bool IsOnBegin() const;

		glm::vec3 GetNext() const;
		glm::vec3 GetCurrent() const;
		glm::vec3 GetPrevious() const;

		glm::vec3 Next();
		glm::vec3 Previous();

		AStarPath& operator=(const AStarPath& other);
	};
}


#endif // !_ASTAR_PATH_H_
