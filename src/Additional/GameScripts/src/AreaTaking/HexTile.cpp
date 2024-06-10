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

float HexTile::_takingStage1 = 33.3f;
float HexTile::_takingStage2 = 66.6f;
float HexTile::_takingStage3 = 100.0f;

void HexTile::TakeOver()
{
	// IF ERROR ONLY (IS OCCUPIED BUT NO OCCUPING ENTITY)
	if (occupyingEntity == nullptr) {
		ResetTile();
		return;
	}


	float takeOverSpeed = occupyingEntity->TakeOverSpeed;
	if (state == TileState::REMOTE_OCCUPYING) {
		takeOverSpeed *= remoteMultiplier;
	}

	if (takenEntity != nullptr && takenEntity != occupyingEntity) {
		percentage -= Time::GetDeltaTime() * takeOverSpeed;
		if (percentage < 0.0f) {
			ResetTile();
		}
	}
	else {
		percentage += Time::GetDeltaTime() * takeOverSpeed;
		if (percentage > 100.0f) {
			percentage = 100.0f;
		}
		if (takenEntity == nullptr && percentage >= _takingStage1)
		{
			takenEntity = occupyingEntity;
			takenEntity->OwnTiles.push_back(this);

			CheckRoundPattern();

			for (auto& t : takenEntity->OwnTiles)
			{
				t->UpdateBorders();

				// Update Neightbours
				vector<GameObject*> neightbours;
				neightbours.resize(6);
				_mapHexTile->tile->GetAdjacentGameObjects(neightbours.data());
				for (auto& nt : neightbours)
				{
					if (nt == nullptr) continue;
					HexTile* ntTile = nt->GetComponent<HexTile>();
					if (ntTile->takenEntity == takenEntity) continue;
					ntTile->UpdateBorders();
				}
			}

			UpdateBorderColor();
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
	TILE_COLOR col = takenEntity != nullptr ? (TILE_COLOR)(uint8_t)(takenEntity->colorIdx == 0 ? 1 : powf(2.f, (float)(takenEntity->colorIdx))) : TILE_COLOR::NEUTRAL;
	//SPDLOG_INFO("Percentage: {}", percentage);
	if (percentage < _takingStage1)
	{
		_meshRenderer->SetMaterial(0, texturesData->GetMaterial(col, 0));
	}
	else if (percentage < _takingStage2)
	{
		_meshRenderer->SetMaterial(0, texturesData->GetMaterial(col, 1));
	}
	else if (percentage < _takingStage3)
	{
		_meshRenderer->SetMaterial(0, texturesData->GetMaterial(col, 2));
	}
	else
	{
		_meshRenderer->SetMaterial(0, texturesData->GetMaterial(col, 3));
	}
}

void HexTile::UpdateBorderColor()
{
	TILE_COLOR col = takenEntity != nullptr ? (TILE_COLOR)(uint8_t)(takenEntity->colorIdx == 0 ? 1 : powf(2.f, (float)(takenEntity->colorIdx))) : TILE_COLOR::NEUTRAL;
	for (auto& b : borders) {
		MeshRenderer* mr = b->GetComponent<MeshRenderer>();
		if (mr != nullptr) {
			mr->SetMaterial(0, texturesData->GetBorderMaterial(col));
		}
	}

	for (auto& bj : borderJoints) {
		MeshRenderer* mr = bj->GetComponent<MeshRenderer>();
		if (mr != nullptr) {
			mr->SetMaterial(0, texturesData->GetBorderMaterial(col));
		}
	}
}

void HexTile::UpdateBorders()
{
	/*borders[0] = TopLeftBorder;
	borders[1] = TopRightBorder;
	borders[2] = MiddleRightBorder;
	borders[3] = BottomRightBorder;
	borders[4] = BottomLeftBorder;
	borders[5] = MiddleLeftBorder;

	borderJoints[0] = TopLeftLeftBorderJoint;
	borderJoints[1] = TopLeftRightBorderJoint;
	borderJoints[2] = TopRightLeftBorderJoint;
	borderJoints[3] = TopRightRightBorderJoint;
	borderJoints[4] = MiddleRightLeftBorderJoint;
	borderJoints[5] = MiddleRightRightBorderJoint;
	borderJoints[6] = BottomRightLeftBorderJoint;
	borderJoints[7] = BottomRightRightBorderJoint;
	borderJoints[8] = BottomLeftLeftBorderJoint;
	borderJoints[9] = BottomLeftRightBorderJoint;
	borderJoints[10] = MiddleLeftLeftBorderJoint;
	borderJoints[11] = MiddleLeftRightBorderJoint;*/

	for (size_t i = 0; i < 6; ++i)
	{
		borders[i]->SetActive(false);
		borderJoints[i * 2]->SetActive(false);
		borderJoints[(i * 2) + 1]->SetActive(false);
	}

	if (takenEntity != nullptr)
	{
		//Color borderCol = takenEntity.Color;
		//Color.RGBToHSV(borderCol, out float h, out float s, out float v);
		//v *= .5f;
		//borderCol = Color.HSVToRGB(h, s, v);

		vector<GameObject*> neightbours;
		neightbours.resize(6);
		_mapHexTile->tile->GetAdjacentGameObjects(neightbours.data());
		for (size_t i = 0; i < 6; ++i)
		{
			GameObject* neightbour = neightbours[i];
			if (neightbour != nullptr)
			{
				HexTile* t = neightbour->GetComponent<HexTile>();
				if (t->takenEntity != takenEntity)
				{
					borders[i]->SetActive(true);
					//borders[i]->GetComponent<MeshRenderer>().material.color = borderCol;
				}
				else
				{
					int left = i * 2 - 1;
					if (left < 0) left = 12 + left;
					borderJoints[left]->SetActive(true);
					borderJoints[(i * 2 + 2) % 12]->SetActive(true);

					//borderJoints[left].GetComponent<MeshRenderer>().material.color = borderCol;
					//borderJoints[(i * 2 + 2) % 12].GetComponent<MeshRenderer>().material.color = borderCol;
				}
			}
			else
			{
				borders[i]->SetActive(true);
				//borders[i].GetComponent<MeshRenderer>().material.color = borderCol;
			}
		}
	}
}


void HexTile::CheckRoundPattern()
{
	HexTile* adjacentHexTile = nullptr;
	Playable* processedTaken = nullptr;

	for (size_t index = 0ull; index < _adjacentTiles.size(); ++index)
	{
		adjacentHexTile = _adjacentTiles[index];

		if (adjacentHexTile->takenEntity == takenEntity || !adjacentHexTile->takenEntity || takenEntity == adjacentHexTile->occupyingEntity)
		{
			processedTaken = nullptr;
			break;
		}

		if (!processedTaken)
		{
			processedTaken = adjacentHexTile->takenEntity;
		}
		else if (processedTaken != adjacentHexTile->takenEntity)
		{
			processedTaken = nullptr;
			break;
		}
	}

	if (processedTaken)
	{
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
}

void HexTile::Initialize()
{
	_mapHexTile = GetGameObject()->GetComponent<MapHexTile>();
	_meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();

	UpdateBorders();
}

void HexTile::OnDestroy()
{
	if (ConcertRoad::instance != nullptr)
		ConcertRoad::instance->RoadMapPoints.erase(this);
	else 
		SPDLOG_WARN("Concert Road Instance was nullptr!");
	//textuesData = nullptr;
}

void HexTile::Update()
{
	if (GameManager::instance == nullptr) return;

	if (!minigameActive && !GameManager::instance->minigameActive && _mapHexTile->type != MapHexTile::HexTileType::Mountain && !isFighting && GameManager::instance->gameStarted && !GameManager::instance->gameOver)
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

void HexTile::InitializeAdjacentTiles()
{
	GameObject* adjacentGameObjects[6];
	HexTile* adjacentHexTile = nullptr;

	_adjacentTiles.reserve(6ull);

	_mapHexTile->tile->GetAdjacentGameObjects(adjacentGameObjects);

	for (size_t index = 0ull; index < 6ull; ++index)
	{
		if (!adjacentGameObjects[index]) continue;

		adjacentHexTile = adjacentGameObjects[index]->GetComponent<HexTile>();

		if (adjacentHexTile->_mapHexTile->type == Generation::MapHexTile::HexTileType::Water
			|| adjacentHexTile->_mapHexTile->type == Generation::MapHexTile::HexTileType::Mountain) continue;

		_adjacentTiles.push_back(adjacentHexTile);
	}

	_adjacentTiles.shrink_to_fit();
}

void HexTile::ResetTile()
{
	percentage = 0.0f;
	if (takenEntity != nullptr) {
		takenEntity->OwnTiles.remove(this);
		for (auto& t : takenEntity->OwnTiles)
		{
			t->UpdateBorders();
		}
	}
	occupyingEntity = nullptr;
	takenEntity = nullptr;
	isFighting = false;
	state = TileState::NONE;
	UpdateTileColor();
	UpdateBorderColor();
	UpdateBorders();
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

	if (texturesData != nullptr) {
		node["textuesData"] = Twin2Engine::Manager::ScriptableObjectManager::GetPath(texturesData->GetId());
	}
	else {
		node["textuesData"] = "";
	}

	for (auto& obj : borders) {
		node["borders"].push_back(obj->Id());
	}

	for (auto& obj : borderJoints) {
		node["borderJoints"].push_back(obj->Id());
	}

	return node;
}

bool HexTile::Deserialize(const YAML::Node& node)
{
	if (!Component::Deserialize(node))
		return false;

	loseInfluenceSpeed = node["loseInfluenceSpeed"].as<float>();
	texturesData = dynamic_cast<HexTileTextureData*>(Twin2Engine::Manager::ScriptableObjectManager::Load(node["textuesData"].as<string>()));

	if (node["borders"]) {
		borders.clear();
		borders.reserve(node["borders"].size());
		for (auto& b : node["borders"]) {
			borders.push_back(SceneManager::GetGameObjectWithId(b.as<size_t>()));
		}
	}

	if (node["borderJoints"]) {
		borderJoints.clear();
		borderJoints.reserve(node["borderJoints"].size());
		for (auto& bj : node["borderJoints"]) {
			borderJoints.push_back(SceneManager::GetGameObjectWithId(bj.as<size_t>()));
		}
	}

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

		/* std::vector<GameObject*> gameObjects = SceneManager::GetAllGameObjects();
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
		GAMEOBJECTS_LIST(TopLeftRightBorderJoint);*/

		// TODO: Zrobic
	}
}

#endif