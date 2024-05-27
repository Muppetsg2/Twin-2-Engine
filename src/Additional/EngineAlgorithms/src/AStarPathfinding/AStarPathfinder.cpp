#include <AstarPathfinding/AStarPathfinder.h>

using namespace AStar;
using namespace Twin2DataStrctures;

using namespace glm;
using namespace std;


vector<AStarPathfindingNode*> AStarPathfinder::_registeredNodes;

//unordered_map<AStarPathfindingNode*, vector<AStarPathfindingNode*>> AStarPathfinder::_nodesGraph;
unordered_map<AStarPathfindingNode*, vector<AStarPathfinder::AStarTargetNodeInfo>> AStarPathfinder::_nodesGraph;


unordered_map<size_t, jthread> AStarPathfinder::_pathfindingThreads;
unordered_map<size_t, bool*> AStarPathfinder::_pathfindingThreadsSearchingPtrs;

mutex AStarPathfinder::_endedThreadsMutex;
list<size_t> AStarPathfinder::_endedThreads;

float AStarPathfinder::_maxMappingDistance = 0.0f;
bool AStarPathfinder::_needsRemapping = true;

void AStarPathfinder::Register(AStarPathfindingNode* node)
{
	if (node)
	{
		//SPDLOG_INFO("Registering");

		bool canBeRegistered = true;

		for (size_t index = 0ull; index < _registeredNodes.size(); ++index)
		{
			if (_registeredNodes[index] == node)
			{
				canBeRegistered = false;
				break;
			}
		}

		
		if (canBeRegistered)
		{
			_registeredNodes.push_back(node);

			_needsRemapping = true;

			// TODO: Dodaæ deynamiczne mapowanie dodawanego node'a
		}
	}
}

void AStarPathfinder::Unregister(AStarPathfindingNode* node)
{
	if (node)
	{
		for (size_t index = 0ull; index < _registeredNodes.size(); ++index)
		{
			if (_registeredNodes[index] == node)
			{
				//SPDLOG_INFO("Unregistering Node in");
				_registeredNodes.erase(_registeredNodes.cbegin() + index);

				if (_nodesGraph.contains(node))
				{
					for (size_t index = 0ull; index < _nodesGraph[node].size(); ++index)
					{
						//Usuwanie powi¹zañ z innych wêz³ów
						AStarTargetNodeInfo& targetNode = _nodesGraph[node][index];
						vector<AStarTargetNodeInfo>& searchNodes = _nodesGraph[targetNode.targetNode];

						for (size_t index2 = 0ull; index2 < searchNodes.size(); ++index2)
						{
							if (searchNodes[index2].targetNode == node)
							{
								searchNodes.erase(searchNodes.cbegin() + index2);
							}
						}
					}

					_nodesGraph[node].clear();
					_nodesGraph.erase(node);
				}

				_needsRemapping = true;
				break;
			}
		}
	}
}


void AStarPathfinder::RemapNodes()
{
	for (auto& pair : _nodesGraph)
	{
		pair.second.clear();
	}
	_nodesGraph.clear();

	SPDLOG_INFO("REMAPPING");

	const size_t size = _registeredNodes.size();
	vec3 currentPos;
	vec3 targetPos;
	float distance;

	for (size_t i = 0ull; i < size; ++i)
	{
		currentPos = _registeredNodes[i]->GetTransform()->GetGlobalPosition();
		currentPos.y = 0.0f;
		//SPDLOG_INFO("REMAPPING Target pos: {} {} {}", currentPos.x, currentPos.y, currentPos.z);

		for (size_t j = i + 1ull; j < size; ++j)
		{
			targetPos = _registeredNodes[j]->GetTransform()->GetGlobalPosition();
			targetPos.y = 0.0f;

			distance = glm::distance(currentPos, targetPos);
			if (distance <= _maxMappingDistance)
			{
				_nodesGraph[_registeredNodes[i]].emplace_back(_registeredNodes[j], targetPos, distance);
				_nodesGraph[_registeredNodes[j]].emplace_back(_registeredNodes[i], currentPos, distance);
			}
		}
	}

	_needsRemapping = false;
}

AStarPathfindingNode* AStarPathfinder::FindClosestNode(vec3 position)
{
	size_t size = _registeredNodes.size();

	if (!size) return nullptr;

	position.y = 0.0f;

	AStarPathfindingNode* closestNode = _registeredNodes[0ull];
	vec3 proposedPosition = closestNode->GetTransform()->GetGlobalPosition();
	proposedPosition.y = 0.0f;
	float minDistance = glm::distance(position, proposedPosition);

	for (size_t index = 1ull; index < size; ++index)
	{
		proposedPosition = _registeredNodes[index]->GetTransform()->GetGlobalPosition();
		proposedPosition.y = 0.0f;

		float distance = glm::distance(position, proposedPosition);

		if (distance < minDistance)
		{
			closestNode = _registeredNodes[index];
			minDistance = distance;
		}
	}

	return closestNode;
}

struct AStarNode
{
	AStarPathfindingNode* current;
	AStarNode* previous;
	glm::vec3 position;
	unsigned int hierarchyDepth;
	float costFromStart;

	AStarNode(AStarPathfindingNode* current, AStarNode* previous, const glm::vec3& position, unsigned hierarchyDepth, float costFromStart) :
							current(current), previous(previous), position(position), hierarchyDepth(hierarchyDepth), costFromStart(costFromStart) {};
};

void AStarPathfinder::FindingPath(size_t threadId,
	glm::vec3 beginPosition,
	glm::vec3 endPosition,
	unsigned int maxPathNodesNumber,
	Twin2Engine::Tools::Action<const AStarPath&> success,
	Twin2Engine::Tools::Action<> failure)
{

	PriorityQueue<AStarNode*, float> priorityQueue([](const float& priority1, const float& priority2) -> bool {
		return priority1 < priority2;
		});

	AStarPathfindingNode* closestToBegin = FindClosestNode(beginPosition);
	AStarPathfindingNode* closestToEnd = FindClosestNode(endPosition);

	vector<vec3> path;
	AStarNode* result = nullptr;
	if (closestToBegin == closestToEnd)
	{
		//Ustawienie wartoœci ró¿nej od zera aby by³ ró¿ny od nullptr
		result = (AStarNode*)1;

		vec3 tempPos = closestToBegin->GetTransform()->GetGlobalPosition();
		tempPos.y = 0.0f;
		path.push_back(tempPos);

		path.push_back(endPosition);
	}
	else
	{
		list<vec3> nodesStack;

		unordered_set<AStarPathfindingNode*> usedNodes;



		list<AStarNode*> allocatedNodes;
		vec3 tempPos = closestToBegin->GetTransform()->GetGlobalPosition();
		tempPos.y = 0.0f;
		AStarNode* allocatedNode = new AStarNode(closestToBegin, nullptr, tempPos, 0, 0.0f);
		//allocatedNode->position.y = 0.0f;

		allocatedNodes.push_back(allocatedNode);

		priorityQueue.Enqueue(allocatedNode, 0.0f);

		usedNodes.insert(closestToBegin);

		while (priorityQueue.Count())
		{
			AStarNode* processedNode = priorityQueue.Dequeue();

			if (processedNode->current == closestToEnd)
			{
				result = processedNode;
				break;
			}

			if (maxPathNodesNumber && maxPathNodesNumber == processedNode->hierarchyDepth)
			{
				continue;
			}

			auto& connected = _nodesGraph[processedNode->current];

			size_t size = connected.size();

			for (size_t index = 0ull; index < size; ++index)
			{
				if (!usedNodes.contains(connected[index].targetNode))
				{
					if (connected[index].targetNode->passable)
					{
						allocatedNode = new AStarNode(connected[index].targetNode, processedNode, connected[index].targetPosition,
							processedNode->hierarchyDepth + 1u, processedNode->costFromStart + connected[index].targetDistance);

						allocatedNodes.push_back(allocatedNode);

						//priorityQueue.Enqueue(allocatedNode, connected[index].targetDistance);
						priorityQueue.Enqueue(allocatedNode, allocatedNode->costFromStart + glm::distance(allocatedNode->position, endPosition));
					}

					usedNodes.insert(connected[index].targetNode);
				}
			}
		}

		// Checking if algorithm found path and processing result
		if (result)
		{
			AStarNode* processedNode = result;
			size_t targetSize = result->hierarchyDepth + 1u;
			path.resize(targetSize);

			size_t insertedIndex = result->hierarchyDepth;

			while (processedNode)
			{
				SPDLOG_ERROR("FP: {} {} {}", processedNode->position.x, processedNode->position.y, processedNode->position.z);
				path[insertedIndex] = processedNode->position;
				--insertedIndex;
				processedNode = processedNode->previous;
			}

			if (glm::distance(path[result->hierarchyDepth - 1ull], endPosition) <= glm::distance(path[result->hierarchyDepth], endPosition))
			{
				path[result->hierarchyDepth] = endPosition;
			}
			else
			{
				//path.insert(path.end(), endPosition);
				path.push_back(endPosition);
			}

			if (glm::distance(path[1ull], beginPosition) <= glm::distance(path[0ull], beginPosition))
			{
				path.erase(path.begin());
			}
		}

		// Deallocating allocated memory
		for (auto& node : allocatedNodes)
		{
			delete node;
		}
	}

	//Deleting informations about thread
	
	//SPDLOG_ERROR("Zastanowiæ siê czy mo¿na wywo³aæ delete na thread");

	(*_pathfindingThreadsSearchingPtrs[threadId]) = false;

	//_pathfindingThreadsSearchingPtrs.erase(threadId);
	//_pathfindingThreads.erase(threadId);

	// Returning result
	if (result)
	{
		success(AStarPath(path));
	}
	else
	{
		failure();
	}
}

AStarPathfindingInfo&& AStarPathfinder::FindPath(const glm::vec3& beginPosition,
							   const glm::vec3& endPosition,
							   unsigned int maxPathNodesNumber,
							   Twin2Engine::Tools::Action<const AStarPath&> success,
							   Twin2Engine::Tools::Action<> failure)
{
	if (_pathfindingThreads.size() == numeric_limits<size_t>().max()) 
		return AStarPathfindingInfo(0, nullptr, nullptr);

	if (_needsRemapping)
	{
		RemapNodes();
	}

	static size_t pathFindingThreadsIds = 0;

	while (_pathfindingThreads.contains(++pathFindingThreadsIds));
	size_t threadId = pathFindingThreadsIds;

	bool* searching = new bool(true);

	_pathfindingThreadsSearchingPtrs[threadId] = searching;

	_pathfindingThreads[threadId] = jthread(FindingPath, threadId, beginPosition, endPosition, maxPathNodesNumber, success, failure);


	return AStarPathfindingInfo(threadId, &_pathfindingThreads[threadId], searching);
}

YAML::Node AStarPathfinder::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["type"] = "AStarPathfinder";
	node["maxMappingDistance"] = _maxMappingDistance;

	return node;
}

bool AStarPathfinder::Deserialize(const YAML::Node& node)
{
	if (!node["maxMappingDistance"] || !Component::Deserialize(node))
		return false;

	_maxMappingDistance = node["maxMappingDistance"].as<float>();

	return true;
}

void AStarPathfinder::OnDestroy()
{
	for (auto& pair : _pathfindingThreads)
	{
		if (pair.second.joinable())
			pair.second.join();
	}
	_pathfindingThreads.clear();

	//for (auto& pair : _pathfindingThreadsSearchingPtrs)
	//{
	//	(*pair.second) = false;
	//}
	_pathfindingThreadsSearchingPtrs.clear();

}

#if _DEBUG

bool AStarPathfinder::DrawInheritedFields()
{
	if (Twin2Engine::Core::Component::DrawInheritedFields()) return true;

	string id = string(std::to_string(this->GetId()));
	ImGui::InputFloat(string("MaxMappingDistance##").append(id).c_str(), &_maxMappingDistance);

	if (ImGui::Button("Remap Nodes"))
	{
		RemapNodes();
	}
	return false;
}

void AStarPathfinder::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("AStarPathfinder##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		DrawInheritedFields();
	}
}

#endif