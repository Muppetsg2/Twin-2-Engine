#include <AstarPathfinding/AStarPathfindingNode.h>

using namespace AStar;

void AStarPathfindingNode::Initialize()
{
	AStarPathfinder::Register(this);
}

void AStarPathfindingNode::OnDestroy()
{
	SPDLOG_INFO("Unregistering Node");
	AStarPathfinder::Unregister(this);
}


YAML::Node AStarPathfindingNode::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["type"] = "AStarPathfindingNode";
	node["passable"] = passable;

	return node;
}

bool AStarPathfindingNode::Deserialize(const YAML::Node& node)
{
    if (!node["passable"] || !Component::Deserialize(node)) 
		return false;

	passable = node["passable"].as<bool>();

	return true;
}


#if _DEBUG

void AStarPathfindingNode::DrawInheritedFields()
{
	ImGui::Checkbox("Passable", &passable);
}

void AStarPathfindingNode::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("AStarPathfindingNode##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		DrawInheritedFields();
	}
}

#endif