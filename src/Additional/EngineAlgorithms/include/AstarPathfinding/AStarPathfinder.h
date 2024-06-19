#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>

#include <tools/EventHandler.h>

#include <AstarPathfinding/AStarPathfindingNode.h>
#include <AstarPathfinding/AStarPath.h>
#include <AstarPathfinding/AStarNodePath.h>

#include <Twin2DataStructures/PriorityQueue.h>

namespace AStar
{
	// Potencjalne b��dy:
	// - zmiany grafu node'�w podczas generowania mo�e skutkowa� nie sp�jno�ci� co mo�e skutkowa� b��dami w w�tkach
	// Potencjalne rozwi�zania:
	// - pe�na synchronizacja pathfindingu
	// - od�o�enie zmian w strukturze do momentu zako�czenia w�tk� wyszukuj�cych
	class AStarPathfindingNode;
	class AStarPathfinder;
	class AStarNodePath;

	class AStarPathfindingInfo
	{
		friend class AStarPathfinder;
	private:
		size_t _threadId;
		std::thread* _thread;
		bool* _searching;
	public:
		AStarPathfindingInfo() : _thread(nullptr), _searching(nullptr) { }
		~AStarPathfindingInfo() {
			WaitForFinding();
			//if (IsSearching() && _thread)
			//	_thread->join();
			//if (_searching) delete _searching;
			//_searching = nullptr;
		}
		AStarPathfindingInfo(size_t threadId, std::thread* thread, bool* searching) : _threadId(threadId), _thread(thread), _searching(searching) { }
		AStarPathfindingInfo(const AStarPathfindingInfo&) = delete;
		AStarPathfindingInfo(AStarPathfindingInfo&& other)
		{
			_threadId = other._threadId;
			_thread = other._thread;
			_searching = other._searching;
			other._thread = nullptr;
			other._searching = nullptr;
		}
		AStarPathfindingInfo& operator=(const AStarPathfindingInfo&) = delete;
		AStarPathfindingInfo& operator=(AStarPathfindingInfo&& other)
		{
			_threadId = other._threadId;
			_thread = other._thread;

			if (_searching)
				delete _searching;

			_searching = other._searching;
			other._thread = nullptr;
			other._searching = nullptr;

			return *this;
		}

		void WaitForFinding()
		{
			if (IsSearching() && _thread)
			{
				_thread->join();
				delete _searching;
				_searching = nullptr;
				_thread = nullptr;
			}
		}
		bool IsSearching()
		{
			if (_searching && !(*_searching))
			{
				delete _searching;
				_searching = nullptr;
				_thread = nullptr;
			}
			return _searching;
		}
	};

	class AStarPathfinder : public Twin2Engine::Core::Component
	{
		friend class AStarPathfindingNode;
		friend class AStarPathfindingInfo;

		struct AStarTargetNodeInfo
		{
			AStarPathfindingNode* targetNode;
			glm::vec3 targetPosition;
			float targetDistance;
		};

		static std::vector<AStarPathfindingNode*> _registeredNodes;

		static std::unordered_map<AStarPathfindingNode*, std::vector<AStarTargetNodeInfo>> _nodesGraph;

		static std::unordered_map<size_t, std::thread> _pathfindingThreads;
		static std::unordered_map<size_t, bool*> _pathfindingThreadsSearchingPtrs;

		static std::mutex _endedThreadsMutex;
		static std::list<size_t> _endedThreads;
		static std::unordered_map<size_t, std::tuple<AStarPath, Twin2Engine::Tools::Action<const AStarPath&>, Twin2Engine::Tools::Action<> >> _endedThreadsResults;

		static float _maxMappingDistance;

		static bool _needsRemapping;

		static void Register(AStarPathfindingNode* node);
		static void Unregister(AStarPathfindingNode* node);


		static AStarPathfindingNode* FindClosestNode(glm::vec3 position);

		//static void RemoveThread(std::thread* pathfindingThread);
		static void FindingPath(size_t threadId, glm::vec3 beginPosition, glm::vec3 endPosition, unsigned int maxPathNodesNumber,
			Twin2Engine::Tools::Action<const AStarPath&> success, Twin2Engine::Tools::Action<> failure);
		static void FindingNodePath(size_t threadId, glm::vec3 beginPosition, glm::vec3 endPosition, unsigned int maxPathNodesNumber,
			Twin2Engine::Tools::Action<const AStarNodePath&> success, Twin2Engine::Tools::Action<> failure);
	public:
		static void RemapNodes();

		static AStarPathfindingInfo FindPath(const glm::vec3& beginPosition, const glm::vec3& endPosition, unsigned int maxPathNodesNumber,
								Twin2Engine::Tools::Action<const AStarPath&> success, Twin2Engine::Tools::Action<> failure);
		static AStarPathfindingInfo FindNodePath(const glm::vec3& beginPosition, const glm::vec3& endPosition, unsigned int maxPathNodesNumber,
								Twin2Engine::Tools::Action<const AStarNodePath&> success, Twin2Engine::Tools::Action<> failure);

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

		virtual void Update() override;
		virtual void OnDestroy() override;

#if _DEBUG
	protected:
		virtual bool DrawInheritedFields() override;
	public:
		virtual void DrawEditor() override;
#endif
	};
}