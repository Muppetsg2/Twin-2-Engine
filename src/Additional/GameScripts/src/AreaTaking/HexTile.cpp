#include <AreaTaking/HexTile.h>

#include <Playable.h>
#include <ConcertRoad.h>
#include <UIScripts/MinigameManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Generation;
using namespace glm;
using namespace std;

float HexTile::_takingStage1 = 30.0f;
float HexTile::_takingStage2 = 60.0f;
float HexTile::_takingStage3 = 90.0f;

void HexTile::TakeOver()
{
	if (!occupyingEntity) {
		ResetTile();
		return;
	}

	float takeOverSpeed = occupyingEntity->TakeOverSpeed;
	if (state == TileState::REMOTE_OCCUPYING) {
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

	UpdateTileColor();
}

void HexTile::LoseInfluence()
{
	//currLoseInfluenceDelay -= Time::GetDeltaTime();
	//if (currLoseInfluenceDelay <= 0.0f && percentage > minLosePercentage) {
	//	currLoseInfluenceDelay = 0.0f;
	//	percentage -= Time::GetDeltaTime() * loseInfluenceSpeed;
	//	if (percentage < minLosePercentage) {
	//		percentage = minLosePercentage;
	//	}
	//	UpdateTileColor();
	//}

	percentage -= Time::GetDeltaTime();
	if (percentage < _takingStage1) {
		percentage = 0.0f;
		if (takenEntity) {
			takenEntity->OwnTiles.remove(this);
			takenEntity = nullptr;
		}
	}
}

void HexTile::UpdateTileColor()
{
	TILE_COLOR col = takenEntity != nullptr ? (TILE_COLOR)(uint8_t)(takenEntity->colorIdx == 0 ? 0 : powf(2.f, (float)(takenEntity->colorIdx - 1))) : TILE_COLOR::BLUE;
	//SPDLOG_INFO("Percentage: {}", percentage);
	if (percentage < _takingStage1)
	{
		//_meshRenderer->SetMaterial(0ull, textuesData->_materials[0][0]->GetId());
		_meshRenderer->SetMaterial(0, textuesData->GetMaterial(col, 0));
	}
	else if (percentage < _takingStage2)
	{
		//_meshRenderer->SetMaterial(0ull, textuesData->_materials[takenEntity->colorIdx][1]->GetId());
		_meshRenderer->SetMaterial(0, textuesData->GetMaterial(col, 1));
	}
	else if (percentage < _takingStage3)
	{
		//_meshRenderer->SetMaterial(0ull, textuesData->_materials[takenEntity->colorIdx][2]->GetId());
		_meshRenderer->SetMaterial(0, textuesData->GetMaterial(col, 2));
	}
	else
	{
		//_meshRenderer->SetMaterial(0ull, textuesData->_materials[takenEntity->colorIdx][3]->GetId());
		_meshRenderer->SetMaterial(0, textuesData->GetMaterial(col, 3));
	}

}

void HexTile::UpdateBorders()
{
}


void HexTile::CheckRoundPattern()
{
	SPDLOG_INFO("Checking Round pattern");
	//GameObject[] neightbours = HexMetrics.GetNeighboringGameObjects(tilemap, this.tilemapPosition);
	GameObject* adjacentGameObjects[6];
	//GameObject* adjacentGameObjects2[6];
	HexTile* adjacentHexTile = nullptr;
	//HexTile* adjacentHexTile2 = nullptr;
	//bool isRounded = true;

	_mapHexTile->tile->GetAdjacentGameObjects(adjacentGameObjects);
	
	Playable* processedTaken = nullptr;

	for (size_t index = 0ull; index < 6ull; ++index)
	{
		if (!adjacentGameObjects[index]) continue;

		adjacentHexTile = adjacentGameObjects[index]->GetComponent<HexTile>();
		//if (item == gameObject || !item.TryGetComponent(out HexTile hx)) continue;

		if (adjacentHexTile->_mapHexTile->type == Generation::MapHexTile::HexTileType::Water
			|| adjacentHexTile->_mapHexTile->type == Generation::MapHexTile::HexTileType::Mountain) continue;

		if (adjacentHexTile->takenEntity == takenEntity || !adjacentHexTile->takenEntity || takenEntity == adjacentHexTile->occupyingEntity)
		{
			//isRounded = false;
			processedTaken = nullptr;
			break;
		}

		if (!processedTaken)
		{
			processedTaken = adjacentHexTile->takenEntity;
		}
		else if (processedTaken != adjacentHexTile->takenEntity)
		{
			//isRounded = false;
			processedTaken = nullptr;
			break;
		}

	}
	//if (isRounded)
	if (processedTaken)
	{
		SPDLOG_INFO("Is Rounded");
		//Playable* temp = takenEntity;
		ResetTile();

		SPDLOG_WARN("Nie sprawdzanie w CheckRounding Pattern occupying is Dead");
		//takenEntity->CheckIfDead(occupyingEntity);
	
		if (takenEntity)
		{
			takenEntity->OwnTiles.remove(this);
		}
	
		takenEntity = processedTaken;
		state = TileState::TAKEN;
		takenEntity->OwnTiles.push_back(this);
		percentage = 100.0f;
		UpdateTileColor();
		UpdateBorders();
	}
	//for (size_t index = 0ull; index < 6ull; ++index)
	//{
	//	if (!adjacentGameObjects[index]) continue;
	//
	//	adjacentHexTile = adjacentGameObjects[index]->GetComponent<HexTile>();
	//	//if (item == gameObject || !item.TryGetComponent(out HexTile hx)) continue;
	//
	//	if (adjacentHexTile->_mapHexTile->type == Generation::MapHexTile::HexTileType::Water 
	//		|| adjacentHexTile->_mapHexTile->type == Generation::MapHexTile::HexTileType::Mountain) continue;
	//
	//	if (adjacentHexTile->takenEntity == occupyingEntity || adjacentHexTile->occupyingEntity != nullptr) continue;
	//
	//
	//	//if (adjacentHexTile->takenEntity != occupyingEntity)
	//	//{
	//	//	isRounded = false;
	//	//	break;
	//	//}
	//
	//	adjacentHexTile->_mapHexTile->tile->GetAdjacentGameObjects(adjacentGameObjects2);
	//	isRounded = true;
	//	for (size_t index2 = 0ull; index2 < 6ull; ++index2)
	//	{
	//		if (!adjacentGameObjects2[index2]) continue;
	//	
	//		adjacentHexTile2 = adjacentGameObjects2[index2]->GetComponent<HexTile>();
	//		//if (item == gameObject || !item.TryGetComponent(out HexTile hx)) continue;
	//	
	//		if (adjacentHexTile2->_mapHexTile->type == Generation::MapHexTile::HexTileType::Water
	//			|| adjacentHexTile2->_mapHexTile->type == Generation::MapHexTile::HexTileType::Mountain) continue;
	//	
	//		if (adjacentHexTile2->takenEntity != occupyingEntity)
	//		{
	//			isRounded = false;
	//			break;
	//		}
	//	}
	//
	//	SPDLOG_INFO("Before Is Rounded");
	//	if (isRounded)
	//	{
	//		SPDLOG_INFO("Is Rounded");
	//
	//		Playable* temp = adjacentHexTile->takenEntity;
	//		adjacentHexTile->ResetTile();
	//		temp->CheckIfDead(takenEntity);
	//	
	//		if (adjacentHexTile->takenEntity)
	//		{
	//			adjacentHexTile->takenEntity->OwnTiles.remove(this);
	//		}
	//	
	//		adjacentHexTile->takenEntity = takenEntity;
	//		adjacentHexTile->state = TileState::TAKEN;
	//		adjacentHexTile->takenEntity->OwnTiles.push_back(this);
	//		adjacentHexTile->percentage = 100.0f;
	//		adjacentHexTile->UpdateTileColor();
	//		adjacentHexTile->UpdateBorders();
	//	}
	//}
}

void HexTile::Initialize()
{
	_mapHexTile = GetGameObject()->GetComponent<MapHexTile>();
	_meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();
}

void HexTile::OnDestroy()
{
	ConcertRoad::instance->RoadMapPoints.erase(this);
	textuesData = nullptr;
}

void HexTile::Update()
{
	if (!minigameActive && !GameManager::instance->minigameActive && _mapHexTile->type != MapHexTile::HexTileType::Mountain && !isFighting)
	{
		if (state == TileState::OCCUPIED || state == TileState::REMOTE_OCCUPYING)
		{
			TakeOver();
		}
		else if (state == TileState::TAKEN && !isAlbumActive)
		{
			LoseInfluence();
		}

		CheckRoundPattern();

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
	state = TileState::NONE;
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

void HexTile::StartTakingOver(Playable* entity) {
	SPDLOG_INFO("Starting taking over");

	if (state != TileState::OCCUPIED) {
		state = TileState::OCCUPIED;
		occupyingEntity = entity;
	}
	else if (occupyingEntity != entity && !isFighting) {
		//entity->StartPaperRockScissors(occupyingEntity);
		//occupyingEntity->StartPaperRockScissors(entity);
		GameManager::instance->minigameActive = true;
		isFighting = true;
		MinigameManager::GetLastInstance()->StartMinigame(entity, occupyingEntity);
	}
}

void HexTile::StopTakingOver(Playable* entity)
{
	if ((state == TileState::OCCUPIED || state == TileState::REMOTE_OCCUPYING) && occupyingEntity == entity) {
		occupyingEntity = nullptr;
		if (takenEntity) {
			state = TileState::TAKEN;
			//currLoseInfluenceDelay = loseInfluenceDelay;
		}
		else {
			state = TileState::NONE;
		}
	}
}

void HexTile::StartRemotelyTakingOver(Playable* entity, float multiplier)
{
	//SPDLOG_INFO("Starting remotely {}", multiplier);
	//if (state != TileState::Occupied && state != TileState::RemoteOccupying) {
	//	state = TileState::RemoteOccupying;
	//
	//	SPDLOG_INFO("In Starting remotely");

	if (state != TileState::OCCUPIED && state != TileState::REMOTE_OCCUPYING) {
		state = TileState::REMOTE_OCCUPYING;

		occupyingEntity = entity;
		remoteMultiplier = multiplier;
	}
}

void HexTile::StopRemotelyTakingOver(Playable* entity)
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

	node["loseInfluenceSpeed"] = loseInfluenceSpeed;

	if (textuesData != nullptr) {
		node["textuesData"] = Twin2Engine::Manager::ScriptableObjectManager::GetPath(textuesData->GetId());
	}
	else {
		node["textuesData"] = "";
	}

	return node;
}

bool HexTile::Deserialize(const YAML::Node& node)
{
	if (!Component::Deserialize(node))
		return false;

	loseInfluenceSpeed = node["loseInfluenceSpeed"].as<float>();
	textuesData = dynamic_cast<HexTileTextureData*>(Twin2Engine::Manager::ScriptableObjectManager::Load(node["textuesData"].as<string>()));
	return true;
}


#if _DEBUG

void HexTile::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("Hex Tile##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {		
		if (Component::DrawInheritedFields()) return;

		ImGui::BeginDisabled();
		ImGui::Checkbox("IsFighting", &isFighting);
		ImGui::EndDisabled();

		ImGui::TextUnformatted("TakenEntity: ");
		ImGui::SameLine();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("%s", takenEntity != nullptr ? takenEntity->GetGameObject()->GetName().append("/").append(std::to_string(takenEntity->GetGameObject()->Id())).c_str() : "None");
		ImGui::PopFont();
		ImGui::TextUnformatted("OccupyingEntity: ");
		ImGui::SameLine();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("%s", occupyingEntity != nullptr ? occupyingEntity->GetGameObject()->GetName().append("/").append(std::to_string(occupyingEntity->GetGameObject()->Id())).c_str() : "None");
		ImGui::PopFont();

		ImGui::Text("Percentage: %f", percentage);
		ImGui::Text("Current Cooldown: %f", currCooldown);

		ImGui::InputFloat("LoseInfluenceSpeed", &loseInfluenceSpeed);

		// TODO: Zrobic
	}
}

#endif