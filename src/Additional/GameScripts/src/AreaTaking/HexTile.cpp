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

	float multiplayer = 1.0f;
	for (size_t index = 0ull; index < _affectingCities.size(); ++index)
	{
		multiplayer *= _affectingCities[index]->CalculateTakingOverSpeedMultiplier(this);
	}

	if (ownerEntity)
	{
		if (multiplayer > 1.0f)
		{
			//pgCity->textureId = _textureCityStar;
			pgCity->active = true;
			pgCityNegative->active = false;
		}
		else if (multiplayer < 1.0f)
		{
			//pgCity->textureId = _textureCityBlackStar;
			pgCity->active = false;
			pgCityNegative->active = true;
		}
		else
		{
			pgCity->active = false;
			pgCityNegative->active = false;
		}
	}
	else
	{
		pgCity->active = false;
		pgCityNegative->active = false;
	}

	takeOverSpeed *= multiplayer;

	if (ownerEntity != nullptr && ownerEntity != occupyingEntity) {
		percentage -= Time::GetDeltaTime() * takeOverSpeed;
		if (percentage <= _takingStage1) {
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

			//CheckRoundPattern();
		}
	}

	UpdateTileColor();
}

void HexTile::LoseInfluence()
{
	float multiplayer = 1.0f;
	for (size_t index = 0ull; index < _affectingCities.size(); ++index)
	{
		multiplayer *= _affectingCities[index]->CalculateLooseInterestMultiplier(this);
	}


	if (ownerEntity)
	{
		if (multiplayer < 1.0f)
		{
			pgCity->active = true;
			pgCityNegative->active = false;
		}
		else if (multiplayer > 1.0f)
		{
			pgCity->active = false;
			pgCityNegative->active = true;
		}
		else
		{
			pgCity->active = false;
			pgCityNegative->active = false;
		}
	}
	else
	{
		pgCity->active = false;
		pgCityNegative->active = false;
	}

	percentage -= multiplayer * Time::GetDeltaTime();

	if (percentage < _takingStage1) {
		percentage = _takingStage1;
		//if (ownerEntity != nullptr) {
		//	ResetTile();
		//}
	}

	UpdateTileColor();
}

void HexTile::UpdateTileColor()
{
	//TILE_COLOR col = ownerEntity != nullptr ? (TILE_COLOR)(uint8_t)(ownerEntity->colorIdx == 0 ? 1 : powf(2.f, (float)(ownerEntity->colorIdx))) : TILE_COLOR::NEUTRAL;
	TILE_COLOR col = ownerEntity != nullptr ? (TILE_COLOR)(uint8_t)(1 << ownerEntity->colorIdx) : TILE_COLOR::NEUTRAL;
	//SPDLOG_INFO("Percentage: {}", percentage);
	if (!occupyingEntity || ownerEntity == occupyingEntity)
	{
		if (percentage >= _takingStage3)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetMaterial(col, 3ull));
		}
		else if (percentage >= _takingStage2)
		{
			_meshRenderer->SetMaterial(0ull, texturesData->GetMaterial(col, 2ull));
		}
		else if (percentage >= _takingStage1)
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
					borderJoints[left]->SetActive(false);
					borderJoints[right]->SetActive(false);
					borders[i]->SetActive(true);
				}
				// JESLI BIALY
				else {
					borderJoints[left]->SetActive(false);
					borderJoints[right]->SetActive(false);
					borders[i]->SetActive(false);
				}

				// UPDATE NEIGHTBOUR BORDER
				// JESLI NIE BIALY
				if (nt->ownerEntity != nullptr) {
					nt->borderJoints[nLeft]->SetActive(false);
					nt->borderJoints[nRight]->SetActive(false);
					nt->borders[ni]->SetActive(true);
				}
				// JESLI BIALY
				else {
					nt->borderJoints[nLeft]->SetActive(false);
					nt->borderJoints[nRight]->SetActive(false);
					nt->borders[ni]->SetActive(false);
				}
			}
			// JESLI DO TEGO SAMEGO GRACZA NALEZA
			else if (ownerEntity != nullptr)
			{
				// UPDATE THIS BORDER
				borderJoints[left]->SetActive(true);
				borderJoints[right]->SetActive(true);
				borders[i]->SetActive(false);

				// UPDATE NEIGHTBOUR BORDER
				nt->borderJoints[nLeft]->SetActive(true);
				nt->borderJoints[nRight]->SetActive(true);
				nt->borders[ni]->SetActive(false);
			}
			// JESLI OBA BIALE
			else {
				// UPDATE THIS BORDER
				borderJoints[left]->SetActive(false);
				borderJoints[right]->SetActive(false);
				borders[i]->SetActive(false);

				// UPDATE NEIGHTBOUR BORDER
				nt->borderJoints[nLeft]->SetActive(false);
				nt->borderJoints[nRight]->SetActive(false);
				nt->borders[ni]->SetActive(false);
			}
		}
		else
		{
			// JESLI KOLOROWY TILE
			if (ownerEntity != nullptr) {
				// UPDATE THIS BORDER
				borderJoints[left]->SetActive(false);
				borderJoints[right]->SetActive(false);
				borders[i]->SetActive(true);
			}
			// JESLI BIALY TILE
			else {
				// UPDATE THIS BORDER
				borderJoints[left]->SetActive(false);
				borderJoints[right]->SetActive(false);
				borders[i]->SetActive(false);
			}
		}
	}
	neightbours.clear();
}

void HexTile::CheckRoundPattern()
{
	if (occupyingEntity)
	{
		return;
	}

	HexTile* adjacentHexTile = nullptr;
	Playable* processedTaken = nullptr;

	for (size_t index = 0ull; index < _adjacentTiles.size(); ++index)
	{
		adjacentHexTile = _adjacentTiles[index];

		if (!adjacentHexTile->ownerEntity
			|| adjacentHexTile->ownerEntity == ownerEntity
			|| (adjacentHexTile->occupyingEntity && ownerEntity == adjacentHexTile->occupyingEntity))
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
		if (percentage <= _takingStage1)
			percentage = 0.5f * (_takingStage1 + _takingStage2);
		//percentage = 100.0f;
		//DisableAlbumAffected();
		UpdateTileColor();
	}
}

void HexTile::CheckNeigbhboursRoundPattern()
{
	for (size_t index = 0ull; index < _adjacentTiles.size(); ++index)
	{
		_adjacentTiles[index]->CheckRoundPattern();
	}
}

void HexTile::Initialize()
{
	_affectingCities.clear();

	_mapHexTile = GetGameObject()->GetComponent<MapHexTile>();
	_meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();

	for (size_t i = 0; i < 6; ++i)
	{
		borderJoints[i * 2]->SetActive(false);
		borderJoints[(i * 2) + 1]->SetActive(false);
		borders[i]->SetActive(false);
	}

	particleGenerator = new ParticleGenerator("origin/ParticleShader", "res/textures/ArrowParticle.png", 3, 0.5f, 0.0f, 6.0f, 2.0f, 0.16f, 0.2f, 0.3f);
	pgCity = new ParticleGenerator("origin/ParticleShader", TextureManager::GetTexture2D(_textureCityStar)->GetId(), 5, 0.3f, 0.0f, 6.0f, 2.0f, 0.12f, 0.12f, 0.4f);
	pgCityNegative = new ParticleGenerator("origin/ParticleShader", TextureManager::GetTexture2D(_textureCityBlackStar)->GetId(), 5, -0.3f, 0.0f, 6.0f, 2.0f, 0.12f, 0.12f, 0.4f);
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

	if (particleGenerator) {
		delete particleGenerator;
		particleGenerator = nullptr;
	}
	if (pgCity) {
		delete pgCity;
		pgCity = nullptr;
	}
	if (pgCityNegative) {
		delete pgCityNegative;
		pgCityNegative = nullptr;
	}
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

		//CheckRoundPattern();

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

	particleGenerator->SetStartPosition(GetTransform()->GetGlobalPosition());
	pgCity->SetStartPosition(GetTransform()->GetGlobalPosition());
	pgCityNegative->SetStartPosition(GetTransform()->GetGlobalPosition() + vec3(0.0f, 0.3f, 0.0f));
	//particleGenerator->active = true;
	
	//GetGameObject()->OnActiveChangedEvent.AddCallback([&](GameObject* go) {
	//		particleGenerator->active = go->GetActive();
	//	});
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
		pgCity->active = false;
		pgCityNegative->active = false;
		DisableAlbumAffected();
		ownerEntity = newOwnerEntity;
		if (ownerEntity != nullptr) {
			ownerEntity->OwnTiles.push_back(this);

			for (auto& t : ownerEntity->OwnTiles)
			{
				t->UpdateBorders();
			}

			CheckNeigbhboursRoundPattern();

			particleGenerator->active = ownerEntity->OwnTiles.front()->particleGenerator->active;
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

void HexTile::EnableAlbumAffected()
{
	particleGenerator->active = true;
}

void HexTile::DisableAlbumAffected()
{
	particleGenerator->active = false;
}

void HexTile::AddAffectingCity(City* city)
{
	_affectingCities.push_back(city);
}

void HexTile::RemoveAffectingCity(City* city)
{
	for (size_t index = 0ull; index < _affectingCities.size(); ++index)
	{
		if (_affectingCities[index] == city)
		{
			_affectingCities.erase(_affectingCities.begin() + index);
			break;
		}
	}
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

		if (_mapHexTile->type == MapHexTile::HexTileType::PointOfInterest)
		{
			Transform* transform = GetTransform();
			if (transform->GetChildCount() == 9ull)
			{
				transform->GetChildAt(transform->GetChildCount() - 2ull)->GetGameObject()->GetComponent<MeshRenderer>()->SetIsTransparent(true);
			}
			else
			{
				transform->GetChildAt(transform->GetChildCount() - 1ull)->GetGameObject()->GetComponent<MeshRenderer>()->SetIsTransparent(true);
			}
		}
	}
	else if (occupyingEntity != entity && !isFighting && entity != nullptr) {
		GameManager::instance->minigameActive = true;
		isFighting = true;
		MinigameManager::GetInstance()->StartMinigame(entity, occupyingEntity);
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


		if (_mapHexTile->type == MapHexTile::HexTileType::PointOfInterest)
		{
			Transform* transform = GetTransform();
			if (transform->GetChildCount() == 9ull)
			{
				transform->GetChildAt(transform->GetChildCount() - 2ull)->GetGameObject()->GetComponent<MeshRenderer>()->SetIsTransparent(false);
			}
			else
			{
				transform->GetChildAt(transform->GetChildCount() - 1ull)->GetGameObject()->GetComponent<MeshRenderer>()->SetIsTransparent(false);
			}
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

	node["_textureCityStar"] = SceneManager::GetTexture2DSaveIdx(_textureCityStar);
	node["_textureCityBlackStar"] = SceneManager::GetTexture2DSaveIdx(_textureCityBlackStar);
	//node["_textureCityStar"] = TextureManager::GetTexture2DPath(_textureCityStar);
	//node["_textureCityBlackStar"] = TextureManager::GetTexture2DPath(_textureCityBlackStar);

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

	_textureCityStar = SceneManager::GetTexture2D(node["textureCityStar"].as<size_t>());
	_textureCityBlackStar = SceneManager::GetTexture2D(node["textureCityBlackStar"].as<size_t>());
	//_textureCityStar = TextureManager::LoadTexture2D(node["textureCityStar"].as<string>())->GetId();
	//_textureCityBlackStar = TextureManager::LoadTexture2D(node["textureCityBlackStar"].as<string>())->GetId();

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