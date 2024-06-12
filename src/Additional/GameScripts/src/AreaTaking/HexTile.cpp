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

float HexTile::_takingStage1 = 0.f;
float HexTile::_takingStage2 = 50.f;
float HexTile::_takingStage3 = 90.0f;

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

	if (ownerEntity != nullptr && ownerEntity != occupyingEntity) {
		percentage -= Time::GetDeltaTime() * takeOverSpeed;
		if (percentage <= 0.f) {
			percentage = 0.f;
			SetOwnerEntity(nullptr);
		}
	}
	else {
		percentage += Time::GetDeltaTime() * takeOverSpeed;
		if (percentage > 100.0f) {
			percentage = 100.0f;
		}
		if (ownerEntity == nullptr && percentage > _takingStage1)
		{
			SetOwnerEntity(occupyingEntity);

			CheckRoundPattern();
		}
	}

	UpdateTileColor();
}

void HexTile::LoseInfluence()
{
	percentage -= Time::GetDeltaTime();
	if (percentage < _takingStage1) {
		percentage = 0.0f;
		if (ownerEntity != nullptr) {
			ResetTile();
		}
	}

	UpdateTileColor();
}

void HexTile::UpdateTileColor()
{
	TILE_COLOR col = ownerEntity != nullptr ? (TILE_COLOR)(uint8_t)(ownerEntity->colorIdx == 0 ? 1 : powf(2.f, (float)(ownerEntity->colorIdx))) : TILE_COLOR::NEUTRAL;
	//SPDLOG_INFO("Percentage: {}", percentage);
	if (!occupyingEntity || ownerEntity == occupyingEntity)
	{
		if (percentage > _takingStage3)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetMaterial(col, 3ull));
		}
		else if (percentage > _takingStage2)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetMaterial(col, 2ull));
		}
		else if (percentage > _takingStage1)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetMaterial(col, 1ull));
		}
		else
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetMaterial(col, 0ull));
		}
	}
	else 
	{
		if (percentage < _takingStage1)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetRetreatingMaterial(col, 0ull));
		}
		else if (percentage < _takingStage2)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetRetreatingMaterial(col, 1ull));
		}
		else if (percentage < _takingStage3)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetRetreatingMaterial(col, 2ull));
		}
		else
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetMaterial(col, 3ull));
		}
	}
}
int HexTile::GetStage() const
{
	if (percentage < _takingStage1)
	{
		return 0;
	}
	else if (percentage < _takingStage2)
	{
		return 1;
	}
	else if (percentage < _takingStage3)
	{
		return 2;
	}
	return 3;
}

void HexTile::UpdateBorderColor()
{
	TILE_COLOR col = ownerEntity != nullptr ? (TILE_COLOR)(uint8_t)(ownerEntity->colorIdx == 0 ? 1 : powf(2.f, (float)(ownerEntity->colorIdx))) : TILE_COLOR::NEUTRAL;
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


	/*for (size_t i = 0; i < 6; ++i)
	{
		borders[i]->SetActive(false);
		borderJoints[i * 2]->SetActive(false);
		borderJoints[(i * 2) + 1]->SetActive(false);
	}*/

	vector<GameObject*> neightbours;
	neightbours.resize(6);
	_mapHexTile->tile->GetAdjacentGameObjects(neightbours.data());
	for (size_t i = 0; i < 6; ++i)
	{
		GameObject* neightbour = neightbours[i];
		int left = (i * 2 + 11) % 12; // -1
		int right = (i * 2 + 2) % 12;

		int ni = (i + 3) % 6;
		int nLeft = (ni * 2 + 11) % 12;
		int nRight = (ni * 2 + 2) % 12;
		if (neightbour != nullptr)
		{
			HexTile* nt = neightbour->GetComponent<HexTile>();

			// JESLI DO INNYCH GRACZY NA LEZA
			if (nt->ownerEntity != ownerEntity)
			{
				// UPDATE THIS BORDER
				// JESLI NIE BIALY
				if (ownerEntity != nullptr) {
					if (borderJoints[left]->GetActive())
						borderJoints[left]->SetActive(false);
					if (borderJoints[right]->GetActive())
						borderJoints[right]->SetActive(false);

					if (!borders[i]->GetActive())
						borders[i]->SetActive(true);
				}
				// JESLI BIALY
				else {
					if (borderJoints[left]->GetActive())
						borderJoints[left]->SetActive(false);
					if (borderJoints[right]->GetActive())
						borderJoints[right]->SetActive(false);

					if (borders[i]->GetActive())
						borders[i]->SetActive(false);
				}

				// UPDATE NEIGHTBOUR BORDER
				// JESLI NIE BIALY
				if (nt->ownerEntity != nullptr) {
					if (nt->borderJoints[nLeft]->GetActive())
						nt->borderJoints[nLeft]->SetActive(false);
					if (nt->borderJoints[nRight]->GetActive())
						nt->borderJoints[nRight]->SetActive(false);

					if (!nt->borders[ni]->GetActive())
						nt->borders[ni]->SetActive(true);
				}
				// JESLI BIALY
				else {
					if (nt->borderJoints[nLeft]->GetActive())
						nt->borderJoints[nLeft]->SetActive(false);
					if (nt->borderJoints[nRight]->GetActive())
						nt->borderJoints[nRight]->SetActive(false);

					if (nt->borders[ni]->GetActive())
						nt->borders[ni]->SetActive(false);
				}
			}
			// JESLI DO TEGO SAMEGO GRACZA NALEZA
			else if (ownerEntity != nullptr)
			{
				// UPDATE THIS BORDER
				if (!borderJoints[left]->GetActive())
					borderJoints[left]->SetActive(true);
				if (!borderJoints[right]->GetActive())
					borderJoints[right]->SetActive(true);

				if (borders[i]->GetActive())
					borders[i]->SetActive(false);

				// UPDATE NEIGHTBOUR BORDER
				if (!nt->borderJoints[nLeft]->GetActive())
					nt->borderJoints[nLeft]->SetActive(true);
				if (!nt->borderJoints[nRight]->GetActive())
					nt->borderJoints[nRight]->SetActive(true);

				if (nt->borders[ni]->GetActive())
					nt->borders[ni]->SetActive(false);
			}
			// JESLI OBA BIALE
			else {
				// UPDATE THIS BORDER
				if (borderJoints[left]->GetActive())
					borderJoints[left]->SetActive(false);
				if (borderJoints[right]->GetActive())
					borderJoints[right]->SetActive(false);

				if (borders[i]->GetActive())
					borders[i]->SetActive(false);

				// UPDATE NEIGHTBOUR BORDER
				if (nt->borderJoints[nLeft]->GetActive())
					nt->borderJoints[nLeft]->SetActive(false);
				if (nt->borderJoints[nRight]->GetActive())
					nt->borderJoints[nRight]->SetActive(false);

				if (nt->borders[ni]->GetActive())
					nt->borders[ni]->SetActive(false);
			}
		}
		else
		{
			// UPDATE THIS BORDER
			if (borderJoints[left]->GetActive())
				borderJoints[left]->SetActive(false);
			if (borderJoints[right]->GetActive())
				borderJoints[right]->SetActive(false);

			if (!borders[i]->GetActive())
				borders[i]->SetActive(true);
		}
	}
	neightbours.clear();
}

void HexTile::CheckRoundPattern()
{
	HexTile* adjacentHexTile = nullptr;
	Playable* processedTaken = nullptr;

	for (size_t index = 0ull; index < _adjacentTiles.size(); ++index)
	{
		adjacentHexTile = _adjacentTiles[index];

		if (adjacentHexTile->ownerEntity == ownerEntity || !adjacentHexTile->ownerEntity || ownerEntity == adjacentHexTile->occupyingEntity)
		{
			processedTaken = nullptr;
			break;
		}

		if (!processedTaken)
		{
			processedTaken = adjacentHexTile->ownerEntity;
		}
		else if (processedTaken != adjacentHexTile->ownerEntity)
		{
			processedTaken = nullptr;
			break;
		}
	}

	if (processedTaken)
	{
		SetOwnerEntity(processedTaken);
		state = TileState::TAKEN;
		if (percentage < _takingStage1)
			percentage = 0.5f * (_takingStage1 + _takingStage2);
		//percentage = 100.0f;
		UpdateTileColor();
	}
}

void HexTile::Initialize()
{
	_mapHexTile = GetGameObject()->GetComponent<MapHexTile>();
	_meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();

	for (size_t i = 0; i < 6; ++i)
	{
		if (borders[i]->GetActive())
			borders[i]->SetActive(false);
		if (borderJoints[i * 2]->GetActive())
			borderJoints[i * 2]->SetActive(false);
		if (borderJoints[(i * 2) + 1]->GetActive())
			borderJoints[(i * 2) + 1]->SetActive(false);
	}
}

void HexTile::OnDestroy()
{
	if (ConcertRoad::instance != nullptr)
	{
		auto found = std::find_if(ConcertRoad::instance->RoadMapPoints.begin(),
								  ConcertRoad::instance->RoadMapPoints.end(),
								  [&](const ConcertRoad::ConcertRoadPoint& point) -> bool { return point.hexTile == this; });

		if (found != ConcertRoad::instance->RoadMapPoints.end())
			ConcertRoad::instance->RoadMapPoints.erase(found);
	}
	else 
		SPDLOG_WARN("Concert Road Instance was nullptr!");
	texturesData = nullptr;

	delete paricleGeneartor;
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

	//paricleGeneartor->startPosition = glm::vec4(GetTransform()->GetGlobalPosition(), 1.0f);
	//paricleGeneartor->active = true;
}

void HexTile::ResetTile()
{
	percentage = 0.0f;
	SetOwnerEntity(nullptr);
	occupyingEntity = nullptr;
	isFighting = false;
	state = TileState::NONE;
	UpdateTileColor();
	UpdateBorderColor();
	UpdateBorders();
}

void HexTile::SetOwnerEntity(Playable* newOwnerEntity)
{
	if (ownerEntity != newOwnerEntity) {
		if (ownerEntity != nullptr) {
			ownerEntity->OwnTiles.remove(this);
			for (auto& t : ownerEntity->OwnTiles)
			{
				t->UpdateBorders();
			}
		}
		ownerEntity = newOwnerEntity;
		if (ownerEntity != nullptr) {
			ownerEntity->OwnTiles.push_back(this);
			for (auto& t : ownerEntity->OwnTiles)
			{
				t->UpdateBorders();
			}
		}

		UpdateBorderColor();
		UpdateTileColor();
	}
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


void HexTile::EnableAffected()
{
	GetTransform()->GetChildAt(0ull)->GetGameObject()->SetActive(true);
}

void HexTile::DisableAffected()
{
	GetTransform()->GetChildAt(0ull)->GetGameObject()->SetActive(false);
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
		GameManager::instance->minigameActive = true;
		isFighting = true;
		MinigameManager::GetLastInstance()->StartMinigame(entity, occupyingEntity);
	}
}

void HexTile::StopTakingOver(Playable* entity)
{
	if ((state == TileState::OCCUPIED || state == TileState::REMOTE_OCCUPYING) && occupyingEntity == entity) {
		occupyingEntity = nullptr;
		if (ownerEntity != nullptr) {
			state = TileState::TAKEN;
		}
		else {
			state = TileState::NONE;
		}
	}
}

void HexTile::StartRemotelyTakingOver(Playable* entity, float multiplier)
{
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

		ImGui::TextUnformatted("OwnerEntity: ");
		ImGui::SameLine();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("%s", ownerEntity != nullptr ? ownerEntity->GetGameObject()->GetName().append("/").append(std::to_string(ownerEntity->GetGameObject()->Id())).c_str() : "None");
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