#include <AreaTaking/HexTile.h>

#include <Playable.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Generation;
using namespace glm;
using namespace std;

static std::vector<std::vector<Material>> _coloredHexTileTextures;

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
	//SPDLOG_INFO("Percentage: {}", percentage);
	if (occupyingEntity)
	{
		if (percentage < 30.0f)
		{
			_meshRenderer->SetMaterial(0ull, textuesData->_materials[occupyingEntity->colorIdx][0].GetId());
		}
		else if (percentage < 60.0f)
		{
			_meshRenderer->SetMaterial(0ull, textuesData->_materials[occupyingEntity->colorIdx][1].GetId());
			takenEntity = occupyingEntity;
		}
		else if (percentage < 80.0f)
		{
			_meshRenderer->SetMaterial(0ull, textuesData->_materials[occupyingEntity->colorIdx][2].GetId());
			takenEntity = occupyingEntity;
		}
		else
		{
			_meshRenderer->SetMaterial(0ull, textuesData->_materials[occupyingEntity->colorIdx][3].GetId());
			takenEntity = occupyingEntity;
		}
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

	if (state != TileState::OCCUPIED) {
		state = TileState::OCCUPIED;
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
	if (state != TileState::OCCUPIED && state != TileState::REMOTE_OCCUPYING) {
		state = TileState::REMOTE_OCCUPYING;
		occupyingEntity = entity;
		remoteMultiplier = multiplier;
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
