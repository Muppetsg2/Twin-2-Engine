#include <AreaTaking/HexTile.h>

#include <Playable.h>
#include <ConcertRoad.h>
#include <UIScripts/MinigameManager.h>
#include <tools/macros.h>
#include <manager/SceneManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
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

	//// Przetworzenie oddzialywania tych, ktorzy z dalszego zasiegu oddzialuja.
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
}

void HexTile::Initialize()
{
	_mapHexTile = GetGameObject()->GetComponent<MapHexTile>();
	_meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();
}

void HexTile::OnDestroy()
{
	if (ConcertRoad::instance != nullptr)
		ConcertRoad::instance->RoadMapPoints.erase(this);
	textuesData = nullptr;
}

void HexTile::Update()
{
	if (GameManager::instance == nullptr) return;

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
			currLoseInfluenceDelay = loseInfluenceDelay;
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

		ImGui::Text("Percentage: %f", percentage);
		ImGui::Text("Current Cooldown: %f", currCooldown);


		std::vector<GameObject*> gameObjects = SceneManager::GetAllGameObjects();
		gameObjects.insert(gameObjects.begin(), nullptr);

#define GAMEOBJECTS_LIST(var)\
		if (ImGui::BeginCombo(concat(#var, "##", id).c_str(), var == nullptr ? "None" : var->GetName().c_str())) {\
			size_t i = 0; bool clicked = false; GameObject* choosed = var;\
			for (auto& item : gameObjects) {\
				if (item != nullptr) if (item->Id() == 0) continue;\
				\
				if (ImGui::Selectable((item == nullptr ? string("None") : item->GetName()).append("##").append(id).append(to_string(i)).c_str(), item == var)) {\
					++i;\
					if (clicked) continue;\
					\
					choosed = item;\
					clicked = true;\
				}\
			}\
			\
			if (clicked) {\
				var = choosed;\
				UpdateBorders();\
			}\
			\
			ImGui::EndCombo();\
		}

		GAMEOBJECTS_LIST(TopRightBorder);
		GAMEOBJECTS_LIST(MiddleRightBorder);
		GAMEOBJECTS_LIST(BottomRightBorder);
		GAMEOBJECTS_LIST(BottomLeftBorder);
		GAMEOBJECTS_LIST(MiddleLeftBorder);
		GAMEOBJECTS_LIST(TopLeftBorder);

		ImGui::Separator();

		GAMEOBJECTS_LIST(TopRightLeftBorderJoint);
		GAMEOBJECTS_LIST(TopRightRightBorderJoint);
		GAMEOBJECTS_LIST(MiddleRightLeftBorderJoint);
		GAMEOBJECTS_LIST(MiddleRightRightBorderJoint);
		GAMEOBJECTS_LIST(BottomRightLeftBorderJoint);
		GAMEOBJECTS_LIST(BottomRightRightBorderJoint);
		GAMEOBJECTS_LIST(BottomLeftLeftBorderJoint);
		GAMEOBJECTS_LIST(BottomLeftRightBorderJoint);
		GAMEOBJECTS_LIST(MiddleLeftLeftBorderJoint);
		GAMEOBJECTS_LIST(MiddleLeftRightBorderJoint);
		GAMEOBJECTS_LIST(TopLeftLeftBorderJoint);
		GAMEOBJECTS_LIST(TopLeftRightBorderJoint);

		// TODO: Zrobic
	}
}

#endif