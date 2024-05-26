#ifndef _ASTAR_PATHFINDER_H_
#define _ASTAR_PATHFINDER_H_

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>

#include <tools/EventHandler.h>

#include <AstarPathfinding/AStarPathfindingNode.h>
#include <AstarPathfinding/AStarPath.h>

#include <Twin2DataStructures/PriorityQueue.h>

namespace AStar
{
	// Potencjalne b��dy:
	// - zmiany grafu node'�w podczas generowania mo�e skutkowa� nie sp�jno�ci� co mo�e skutkowa� b��dami w w�tkach
	// Potencjalne rozwi�zania:
	// - pe�na synchronizacja pathfindingu
	// - od�o�enie zmian w strukturze do momentu zako�czenia w�tk� wyszukuj�cych
	class AStarPathfindingNode;

	class AStarPathfinder : public Twin2Engine::Core::Component
	{
		friend class AStarPathfindingNode;

		struct AStarTargetNodeInfo
		{
			AStarPathfindingNode* targetNode;
			glm::vec3 targetPosition;
			float targetDistance;
		};

		static std::vector<AStarPathfindingNode*> _registeredNodes;

		//static std::unordered_map<AStarPathfindingNode*, std::vector<AStarPathfindingNode*>> _nodesGraph;
		static std::unordered_map<AStarPathfindingNode*, std::vector<AStarTargetNodeInfo>> _nodesGraph;

		static std::unordered_map<size_t, std::thread*> _pathfindingThreads;

		static float _maxMappingDistance;

		static bool _needsRemapping;

		static void Register(AStarPathfindingNode* node);
		static void Unregister(AStarPathfindingNode* node);


		static AStarPathfindingNode* FindClosestNode(glm::vec3 position);

		//static void RemoveThread(std::thread* pathfindingThread);
		static void FindingPath(size_t threadId, glm::vec3 beginPosition, glm::vec3 endPosition, unsigned int maxPathNodesNumber,
			Twin2Engine::Tools::Action<const AStarPath&> success, Twin2Engine::Tools::Action<> failure);
	public:
		static void RemapNodes();

		static bool FindPath(const glm::vec3& beginPosition, const glm::vec3& endPosition, unsigned int maxPathNodesNumber,
								Twin2Engine::Tools::Action<const AStarPath&> success, Twin2Engine::Tools::Action<> failure);

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

#endif // !_ASTAR_PATHFINDER_H_
