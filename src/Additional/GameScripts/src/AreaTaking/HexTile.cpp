#include <AreaTaking/HexTile.h>

using namespace Twin2Engine::Core;
using namespace Generation;
using namespace glm;
using namespace std;

void HexTile::Initialize()
{
	_mapHexTile = GetGameObject()->GetComponent<MapHexTile>();
}

void HexTile::OnDestroy()
{
}

Generation::MapHexTile* HexTile::GetMapHexTile() const
{
	return _mapHexTile;
}

YAML::Node HexTile::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["type"] = "HexTile";

	return node;
}

bool HexTile::Deserialize(const YAML::Node& node)
{
	if (!Component::Deserialize(node))
		return false;


	return true;
}


#if _DEBUG

bool HexTile::DrawInheritedFields()
{
	if (Component::DrawInheritedFields()) return true;
	ImGui::BeginDisabled();
	ImGui::Checkbox("IsFighting", &IsFighting);
	ImGui::EndDisabled();
	if (takenEntity)
	{
		ImGui::Text("TakenEntity: %d", takenEntity->GetGameObject()->Id());
	}
	else
	{
		ImGui::Text("TakenEntity: nullptr");
	}
	ImGui::Text("percentage: %f", percentage);
	ImGui::Text("currCooldown: %f", currCooldown);
	return false;
}

void HexTile::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("Hex Tile##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (DrawInheritedFields()) return;
		// TODO: Zrobic
	}
}

#endif