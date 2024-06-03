#include <AreaTaking/HexTile.h>

#include <Playable.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Generation;
using namespace glm;
using namespace std;

std::vector<std::vector<Material>> HexTile::_coloredHexTileTextures;

float HexTile::_takingStage1 = 30.0f;
float HexTile::_takingStage2 = 60.0f;
float HexTile::_takingStage3 = 90.0f;


void HexTile::Initialize()
{
	_mapHexTile = GetGameObject()->GetComponent<MapHexTile>();
	_meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();
}

void HexTile::OnDestroy()
{
}

void HexTile::Update()
{
	if (!minigameActive && !GameManager::instance->minigameActive && _mapHexTile->type != MapHexTile::HexTileType::Mountain && !isFighting)
	{
		if (state == TileState::Occupied || state == TileState::RemoteOccupying)
		{
			TakeOver();
		}
		else if (state == TileState::Taken && !isAlbumActive)
		{
			LoseInfluence();
		}
	}
}

Generation::MapHexTile* HexTile::GetMapHexTile() const
{
	return _mapHexTile;
}

YAML::Node HexTile::Serialize() const
{
	YAML::Node node = Component::Serialize();

	node["type"] = "HexTile";
	node["textuesData"] = Twin2Engine::Manager::ScriptableObjectManager::GetPath(textuesData->GetId());

	return node;
}

bool HexTile::Deserialize(const YAML::Node& node)
{
	if (!Component::Deserialize(node))
		return false;


	textuesData = dynamic_cast<HexTileTextureData*>(Twin2Engine::Manager::ScriptableObjectManager::Load(node["textuesData"].as<string>()));
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

#endif

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

	if (takenEntity && takenEntity != occupyingEntity) {
		percentage -= Time::GetDeltaTime() * takeOverSpeed;
		if (percentage < _takingStage1) {
			percentage = 0.0f;
			if (takenEntity) {
				takenEntity->OwnTiles.remove(this);
				takenEntity = nullptr;
			}
			CheckRoundPattern();
		}
	}
	else {
		percentage += Time::GetDeltaTime() * takeOverSpeed;
		if (percentage > 100.0f) {
			percentage = 100.0f;
		}
		if (!takenEntity && percentage >= _takingStage1)
		{
			takenEntity = occupyingEntity;
			takenEntity->OwnTiles.push_back(this);

		}
	}

	//// Przetworzenie oddzia�ywania tych, kt�rzy z dalszego zasi�gu oddzia�uj�.
	//size_t size = remotelyOccupyingEntities.size();
	//for (size_t index = 0ull; index < size; ++index)
	//{
	//	float takeOverSpeed = remotelyOccupyingEntities[index]->TakeOverSpeed * remoteMultipliers[index];
	//	if (takenEntity && takenEntity != remotelyOccupyingEntities[index]) {
	//		percentage -= Time::GetDeltaTime() * takeOverSpeed;
	//		if (percentage < _takingStage1) {
	//			percentage = 0.0f;
	//			if (takenEntity) {
	//				takenEntity->OwnTiles.remove(this);
	//				takenEntity = nullptr;
	//			}
	//			CheckRoundPattern();
	//		}
	//	}
	//	else {
	//		percentage += Time::GetDeltaTime() * takeOverSpeed;
	//		if (percentage > 100.0f) {
	//			percentage = 100.0f;
	//		}
	//		if (!takenEntity && percentage >= _takingStage1)
	//		{
	//			takenEntity = remotelyOccupyingEntities[index];
	//			takenEntity->OwnTiles.push_back(this);
	//
	//		}
	//	}
	//}

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
	//SPDLOG_INFO("Percentage: {}", percentage);
	if (percentage < _takingStage1)
	{
		_meshRenderer->SetMaterial(0ull, textuesData->_materials[0][0].GetId());
	}
	else if (percentage < _takingStage2)
	{
		_meshRenderer->SetMaterial(0ull, textuesData->_materials[takenEntity->colorIdx][1].GetId());
	}
	else if (percentage < _takingStage3)
	{
		_meshRenderer->SetMaterial(0ull, textuesData->_materials[takenEntity->colorIdx][2].GetId());
	}
	else
	{
		_meshRenderer->SetMaterial(0ull, textuesData->_materials[takenEntity->colorIdx][3].GetId());
	}

}

void HexTile::UpdateBorders()
{
}

void HexTile::CheckRoundPattern()
{
}

void HexTile::StartTakingOver(Playable* entity) {
	SPDLOG_INFO("Starting taking over");

	if (state != TileState::Occupied) {
		state = TileState::Occupied;
		occupyingEntity = entity;
	}
	else if (occupyingEntity != entity && !isFighting) {
		entity->StartPaperRockScissors(occupyingEntity);
		occupyingEntity->StartPaperRockScissors(entity);
		isFighting = true;
	}
}

void HexTile::StartRemotelyTakingOver(Playable* entity, float multiplier)
{
	SPDLOG_INFO("Starting remotely {}", multiplier);
	if (state != TileState::Occupied && state != TileState::RemoteOccupying) {
		state = TileState::RemoteOccupying;

		SPDLOG_INFO("In Starting remotely");

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
