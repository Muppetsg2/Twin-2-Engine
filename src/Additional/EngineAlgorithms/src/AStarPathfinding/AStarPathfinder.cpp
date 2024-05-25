#include <AstarPathfinding/AStarPathfinder.h>

using namespace AStar;
using namespace Twin2DataStrctures;

using namespace glm;
using namespace std;


vector<AStarPathfindingNode*> AStarPathfinder::_registeredNodes;

//unordered_map<AStarPathfindingNode*, vector<AStarPathfindingNode*>> AStarPathfinder::_nodesGraph;
unordered_map<AStarPathfindingNode*, vector<AStarPathfinder::AStarTargetNodeInfo>> AStarPathfinder::_nodesGraph;


unordered_map<size_t, thread*> AStarPathfinder::_pathfindingThreads;

float AStarPathfinder::_maxMappingDistance = 0.0f;
bool AStarPathfinder::_needsRemapping = true;

void AStarPathfinder::Register(AStarPathfindingNode* node)
{
	if (node)
	{
		bool canBeRegistered = true;

		for (size_t index = 0ull; index < _registeredNodes.size(); ++index)
		{
			if (_registeredNodes[index] = node)
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
			if (_registeredNodes[index] = node)
			{
				_registeredNodes.erase(_registeredNodes.cbegin() + index);

				if (_nodesGraph.contains(node))
				{
					for (size_t index = 0ull; index < _nodesGraph[node].size(); ++index)
					{
						//Usuwanie powi¹zañ z innych wêz³ów
						AStarTargetNodeInfo& targetNode = _nodesGraph[node][index];
						vector<AStarTargetNodeInfo>& searchNodes = _nodesGraph[targetNode.targetNode];

						for (size_t index = 0ull; index < searchNodes.size(); ++index)
						{
							if (searchNodes[index].targetNode = node)
							{
								searchNodes.erase(searchNodes.cbegin() + index);
							}
						}
					}

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

	for (size_t i = 0ull; i < _registeredNodes.size(); ++i)
	{
		vec3 currentPos = _registeredNodes[i]->GetTransform()->GetGlobalPosition();

		for (size_t j = i + 1ull; j < _registeredNodes.size(); ++j)
		{
			vec3 targetPos = _registeredNodes[j]->GetTransform()->GetGlobalPosition();
			float distance = glm::distance(currentPos, targetPos);
			if (distance < _maxMappingDistance)
			{
				_nodesGraph[_registeredNodes[i]].emplace_back(_registeredNodes[j], targetPos, distance);
				_nodesGraph[_registeredNodes[j]].emplace_back(_registeredNodes[i], currentPos, distance);
			}
		}
	}
}

AStarPathfindingNode* AStarPathfinder::FindClosestNode(vec3 position)
{
	size_t size = _registeredNodes.size();
	AStarPathfindingNode* closestNode = _registeredNodes[0ull];
	float minDistance = glm::distance(position, closestNode->GetTransform()->GetGlobalPosition());

	for (size_t index = 1ull; index < size; ++index)
	{
		float distance = glm::distance(position, _registeredNodes[index]->GetTransform()->GetGlobalPosition());

		if (distance < minDistance)
			closestNode = _registeredNodes[index];
	}

	return closestNode;
}

struct AStarNode
{
	AStarPathfindingNode* current;
	AStarNode* previous;
	glm::vec3 position;
	unsigned int hierarchyDepth;

	AStarNode(AStarPathfindingNode* current, AStarNode* previous, const glm::vec3& position, unsigned hierarchyDepth) :
							current(current), previous(previous), position(position), hierarchyDepth(hierarchyDepth) {};
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
	
	list<vec3> nodesStack;

	unordered_set<AStarPathfindingNode*> usedNodes;

	AStarNode* result = nullptr;


	list<AStarNode*> allocatedNodes;
	AStarNode* allocatedNode = new AStarNode(closestToBegin, nullptr, closestToBegin->GetTransform()->GetLocalPosition(), 0);
	allocatedNodes.push_back(allocatedNode);

	priorityQueue.Enqueue(allocatedNode, 0.0f);


	while (priorityQueue.Count())
	{
		AStarNode* processedNode = priorityQueue.Dequeue();

		if (processedNode->current == closestToEnd)
		{
			result = processedNode;
			break;
		}

		auto& connected = _nodesGraph[processedNode->current];

		if (maxPathNodesNumber && maxPathNodesNumber == processedNode->hierarchyDepth)
		{
			continue;
		}

		size_t size = connected.size();

		for (size_t index = 0ull; index < size; ++index)
		{
			if (!usedNodes.contains(connected[index].targetNode))
			{
				allocatedNode = new AStarNode(connected[index].targetNode, processedNode, connected[index].targetPosition, processedNode->hierarchyDepth + 1u);
				allocatedNodes.push_back(allocatedNode);

				priorityQueue.Enqueue(allocatedNode, connected[index].targetDistance);

				usedNodes.insert(connected[index].targetNode);
			}
		}
	}

	// Checking if algorithm found path and processing result
	vector<vec3> path;
	if (result)
	{
		AStarNode* processedNode = result;
		size_t targetSize = result->hierarchyDepth + 1u;
		path.reserve(targetSize);

		size_t insertedIndex = result->hierarchyDepth;

		while (processedNode)
		{
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
			path.insert(path.end(), endPosition);
		}

		if (glm::distance(path[1], beginPosition) <= glm::distance(path[0], beginPosition))
		{
			path.erase(path.begin());
		}
	}

	// Deallocating allocated memory
	for (auto& node : allocatedNodes)
	{
		delete node;
	}

	//Deleting informations about thread
	delete _pathfindingThreads[threadId];
	_pathfindingThreads.erase(threadId);

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

bool AStarPathfinder::FindPath(const glm::vec3& beginPosition,
							   const glm::vec3& endPosition,
							   unsigned int maxPathNodesNumber,
							   Twin2Engine::Tools::Action<const AStarPath&> success,
							   Twin2Engine::Tools::Action<> failure)
{
	if (_pathfindingThreads.size() == numeric_limits<size_t>().max()) 
		return false;

	if (_needsRemapping)
	{
		RemapNodes();
	}

	static size_t pathFindingThreadsIds = 0;
	size_t threadId = pathFindingThreadsIds++;

	while (_pathfindingThreads.contains(threadId)) threadId = pathFindingThreadsIds++;

	thread* pathfindingThread = new thread(FindingPath, threadId, beginPosition, endPosition, maxPathNodesNumber, success, failure);

	_pathfindingThreads[threadId] = pathfindingThread;
}

YAML::Node AStarPathfinder::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["maxMappingDistance"] = _maxMappingDistance;

	return node;
}

bool AStarPathfinder::Deserialize(const YAML::Node& node)
{
	if (!node["maxMappingDistance"] || !Component::Deserialize(node))
		return false;

	_maxMappingDistance = node["maxMappingDistance"].as<bool>();

	return true;
}

#if _DEBUG

void AStarPathfinder::DrawInheritedFields()
{
	ImGui::InputFloat("MaxMappingDistance", &_maxMappingDistance);
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