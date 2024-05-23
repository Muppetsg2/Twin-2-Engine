#include <AstarPathfinding/AStarPathfinder.h>

using namespace AStar;

using namespace glm;
using namespace std;


vector<AStarPathfindingNode*> AStarPathfinder::_registeredNodes;

//unordered_map<AStarPathfindingNode*, vector<AStarPathfindingNode*>> AStarPathfinder::_nodesGraph;
unordered_map<AStarPathfindingNode*, vector<AStarPathfinder::AStarTargetNodeInfo>> AStarPathfinder::_nodesGraph;


unordered_map<size_t, thread*> AStarPathfinder::_pathfindingThreads;

float AStarPathfinder::_maxMappingDistance = 0.0f;

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


void AStarPathfinder::FindingPath(size_t threadId, 
								  glm::vec3 beginPosition,
								  glm::vec3 endPosition,
								  Twin2Engine::Tools::Action<const AStarPath&> success, 
								  Twin2Engine::Tools::Action<> failure)
{


	_pathfindingThreads.erase(threadId);
}

bool AStarPathfinder::FindPath(const glm::vec3& beginPosition,
	const glm::vec3& endPosition,
	Twin2Engine::Tools::Action<const AStarPath&> success,
	Twin2Engine::Tools::Action<> failure)
{
	if (_pathfindingThreads.size() == numeric_limits<size_t>().max()) 
		return false;

	static size_t pathFindingThreadsIds = 0;
	size_t threadId = pathFindingThreadsIds++;

	while (_pathfindingThreads.contains(threadId)) threadId = pathFindingThreadsIds++;

	thread* pathfindingThread = new thread(FindingPath, threadId, beginPosition, endPosition, success, failure);

	_pathfindingThreads[threadId] = pathfindingThread;
}

YAML::Node AStarPathfinder::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["maxMappingDistance"] = _maxMappingDistance;
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