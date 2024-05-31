#include <AreaTaking/HexTile.h>

#include <Playable.h>

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

void HexTile::Update()
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
	ImGui::Checkbox("IsFighting", &isFighting);
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

void HexTile::ResetTile()
{
	percentage = 0.0f;
	if (takenEntity) {
		//takenEntity->RemoveTile(this);
	}
	occupyingEntity = nullptr;
	takenEntity = nullptr;
	isFighting = false;
	state = TileState::None;
}

void HexTile::SetOutlineActive(bool active)
{
	//if (outline) {
	//	outline->SetEnabled(active);
	//}
}

void HexTile::StartMinigame()
{
}

void HexTile::WinMinigame()
{
}

void HexTile::BadNote()
{
	percentage -= badNotePercent;
	if (percentage <= 0.0f) {
		percentage = 0.0f;
	}
}

void HexTile::TakeOver()
{
	if (!occupyingEntity) {
		ResetTile();
		return;
	}

	float takeOverSpeed = occupyingEntity->TakeOverSpeed;
	if (state == TileState::RemoteOccupying) {
		takeOverSpeed *= remoteMultiplier;
	}

	if (takenEntity != occupyingEntity) {
		percentage -= Time::GetDeltaTime() * takeOverSpeed;
		if (percentage <= 0.0f) {
			percentage = 0.0f;
			if (takenEntity) {
				takenEntity->OwnTiles.remove(this);
			}
			takenEntity = occupyingEntity;
			takenEntity->OwnTiles.push_back(this);
			CheckRoundPattern();
		}
	}
	else {
		percentage += Time::GetDeltaTime() * takeOverSpeed;
		if (percentage >= 100.0f) {
			percentage = 100.0f;
		}
	}
	UpdateTileColor();
}

void HexTile::LoseInfluence()
{
	currLoseInfluenceDelay -= Time::GetDeltaTime();
	if (currLoseInfluenceDelay <= 0.0f && percentage > minLosePercentage) {
		currLoseInfluenceDelay = 0.0f;
		percentage -= Time::GetDeltaTime() * loseInfluenceSpeed;
		if (percentage < minLosePercentage) {
			percentage = minLosePercentage;
		}
		UpdateTileColor();
	}
}

void HexTile::UpdateTileColor()
{
}

void HexTile::UpdateBorders()
{
}

void HexTile::CheckRoundPattern()
{
}

void HexTile::StartTakingOver(Playable* entity) {
	if (state != TileState::Occupied) {
		state = TileState::Occupied;
		occupyingEntity = entity;
	}
	else if (occupyingEntity != entity && !isFighting) {
		//entity->StartPaperRockScissors(occupyingEntity);
		//occupyingEntity->StartPaperRockScissors(entity);
		//isFighting = true;
	}
}

void HexTile::StartRemotelyTakingOver(Playable* entity, float multiplier)
{
	if (state != TileState::Occupied && state != TileState::RemoteOccupying) {
		state = TileState::RemoteOccupying;
		occupyingEntity = entity;
		remoteMultiplier = multiplier;
	}
}

void HexTile::StopTakingOver(Playable* entity)
{
	if ((state == TileState::Occupied || state == TileState::RemoteOccupying) && occupyingEntity == entity) {
		occupyingEntity = nullptr;
		if (takenEntity) {
			state = TileState::Taken;
			currLoseInfluenceDelay = loseInfluenceDelay;
		}
		else {
			state = TileState::None;
		}
	}
}

#endif