#ifndef _ASTAR_NODE_H_
#define _ASTAR_NODE_H_

#include <core/Component.h>

#include <AstarPathfinding/AStarPathfinder.h>

namespace AStar
{
	class AStarPathfindingNode : public Twin2Engine::Core::Component
	{

	public:
		bool passable = true;

		virtual void Initialize() override;
		virtual void OnDestroy() override;


		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	protected:
		virtual void DrawInheritedFields() override;
	public:
		virtual void DrawEditor() override;
#endif
	};
}


#endif // !_ASTAR_NODE_H_