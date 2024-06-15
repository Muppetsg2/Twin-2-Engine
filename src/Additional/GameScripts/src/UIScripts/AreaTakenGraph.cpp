#include <UIScripts/AreaTakenGraph.h>

using namespace Twin2Engine::UI;

void AreaTakenGraph::UpdateTopHexagon()
{
	Prefab* topHexagonPrefab = PrefabManager::GetPrefab(_topHexagonPrefabId);
	if (topHexagonPrefab == nullptr) return;

	std::vector<Playable*> entities = GameManager::instance->entities;

	if (entities.size() != _topHexagons.size()) {
		while (_topHexagons.size() < entities.size() + 1) {
			_topHexagons.push_back(SceneManager::CreateGameObject(topHexagonPrefab, GetTransform()));
		}

		while (_topHexagons.size() > entities.size() + 1) {
			SceneManager::DestroyGameObject(_topHexagons[_topHexagons.size() - 1]);
			_topHexagons.erase(_topHexagons.end() - 1);
		}
	}

	std::vector<HexTile*> tiles = GameManager::instance->Tiles;
	float offset = 0.f;
	size_t idx = 0;
	for (auto& entity : entities) {
		float progress = ((float)entity->OwnTiles.size() / tiles.size()) * 100.f;

		Image* img = _topHexagons[idx]->GetComponent<Image>();
		img->SetFillOffset(offset);
		img->SetFillProgress(offset + progress);

		TILE_COLOR col = (TILE_COLOR)(uint8_t)(entity->colorIdx == 0 ? 1 : powf(2.f, (float)(entity->colorIdx)));
		switch (col) {
		case TILE_COLOR::BLUE:
			img->SetColor(glm::vec4(0.31f, 0.361f, 0.886f, 1.f));
			break;
		case TILE_COLOR::CYAN:
			img->SetColor(glm::vec4(0.31f, 0.867f, 0.886f, 1.f));
			break;
		case TILE_COLOR::GREEN:
			img->SetColor(glm::vec4(0.31f, 0.886f, 0.396f, 1.f));
			break;
		case TILE_COLOR::PINK:
			img->SetColor(glm::vec4(0.894f, 0.31f, 0.686f, 1.f));
			break;
		case TILE_COLOR::PURPLE:
			img->SetColor(glm::vec4(0.769f, 0.31f, 0.89f,1.f));
			break;
		case TILE_COLOR::RED:
			img->SetColor(glm::vec4(0.886f, 0.406f, 0.31f, 1.f));
			break;
		case TILE_COLOR::YELLOW:
			img->SetColor(glm::vec4(0.875f, 0.886f, 0.31f, 1.f));
			break;
		case TILE_COLOR::NEUTRAL:
		default:
			img->SetColor(glm::vec4(1.f));
			break;
		}

		offset += progress;
		++idx;
	}

	if (offset < 100.f) {
		_topHexagons[idx]->SetActive(true);
		Image* img = _topHexagons[idx]->GetComponent<Image>();
		img->SetFillOffset(offset);
		img->SetFillProgress(100.f);
		img->SetColor(glm::vec4(1.f));
	}
	else {
		_topHexagons[idx]->SetActive(false);
	}
}

void AreaTakenGraph::UpdateEdge()
{

}

void AreaTakenGraph::UpdateBottomHexagon()
{

}

bool AreaTakenGraph::PrefabDropDown(const char* label, size_t* prefabId, const std::string& objId)
{
	std::map<size_t, string> prefabNames = PrefabManager::GetAllPrefabsNames();

	prefabNames.insert(std::pair(0, "None"));

	size_t prefabNameIdx = 0;
	if (prefabNames.contains(*prefabId)) {
		prefabNameIdx = *prefabId;
	}

	if (ImGui::BeginCombo(label, prefabNames[prefabNameIdx].c_str())) {

		bool clicked = false;
		size_t choosed = prefabNameIdx;
		for (auto& item : prefabNames) {

			if (ImGui::Selectable(item.second.append("##").append(objId).c_str(), item.first == prefabNameIdx)) {

				if (clicked) continue;

				choosed = item.first;
				clicked = true;
			}
		}

		ImGui::EndCombo();

		if (clicked) {
			*prefabId = choosed;
		}

		return clicked;
	}

	return false;
}

void AreaTakenGraph::Update()
{
	if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
		UpdateTopHexagon();
		UpdateEdge();
		UpdateBottomHexagon();
	}
}

void AreaTakenGraph::OnDestroy()
{
	_topHexagons.clear();
	_edges.clear();
	_bottomHexagon = nullptr;
}

YAML::Node AreaTakenGraph::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "AreaTakenGraph";
	node["topCirclePrefab"] = SceneManager::GetPrefabSaveIdx(_topHexagonPrefabId);
	node["edgePrefab"] = SceneManager::GetPrefabSaveIdx(_edgePrefabId);
	node["bottomCirclePrefab"] = SceneManager::GetPrefabSaveIdx(_bottomHexagonPrefabId);
	return node;
}

bool AreaTakenGraph::Deserialize(const YAML::Node& node)
{
	bool isGood = true;

	isGood = isGood && !Component::Deserialize(node);

	if (isGood = isGood && node["topCirclePrefab"]) {
		_topHexagonPrefabId = SceneManager::GetPrefab(node["topCirclePrefab"].as<size_t>());
	}

	if (isGood = isGood && node["edgePrefab"]) {
		_edgePrefabId = SceneManager::GetPrefab(node["edgePrefab"].as<size_t>());
	}

	if (isGood = isGood && node["bottomCirclePrefab"]) {
		_bottomHexagonPrefabId = SceneManager::GetPrefab(node["bottomCirclePrefab"].as<size_t>());
	}

	return isGood;
}

#if _DEBUG
void AreaTakenGraph::DrawEditor() {
	string id = string(std::to_string(this->GetId()));
	string name = string("AreaTakenGraph##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		DrawInheritedFields();

		size_t prefabId = _topHexagonPrefabId;
		if (PrefabDropDown(string("Top Hexagon Prefab##").append(id).c_str(), &prefabId, id)) {
			SetTopHexagonPrefabId(prefabId);
		}

		prefabId = _edgePrefabId;
		if (PrefabDropDown(string("Edge Prefab##").append(id).c_str(), &prefabId, id)) {
			SetEdgePrefabId(prefabId);
		}

		prefabId = _bottomHexagonPrefabId;
		if (PrefabDropDown(string("Bottom Hexagon Prefab##").append(id).c_str(), &prefabId, id)) {
			SetBottomHexagonPrefabId(prefabId);
		}
	}
}
#endif

void AreaTakenGraph::SetTopHexagonPrefabId(size_t prefabId)
{
	if (_topHexagonPrefabId != prefabId) {
		_topHexagonPrefabId = prefabId;
	}
}

void AreaTakenGraph::SetEdgePrefabId(size_t prefabId)
{
	if (_edgePrefabId != prefabId) {
		_edgePrefabId = prefabId;
	}
}

void AreaTakenGraph::SetBottomHexagonPrefabId(size_t prefabId)
{
	if (_bottomHexagonPrefabId != prefabId) {
		_bottomHexagonPrefabId = prefabId;
	}
}

size_t AreaTakenGraph::GetTopHexagonPrefabId() const
{
	return _topHexagonPrefabId;
}

size_t AreaTakenGraph::GetEdgePrefabId() const
{
	return _edgePrefabId;
}

size_t AreaTakenGraph::GetBottomHexagonPrefabId() const
{
	return _bottomHexagonPrefabId;
}

std::vector<GameObject*> AreaTakenGraph::GetTopHexagons() const
{
	return _topHexagons;
}

std::vector<GameObject*> AreaTakenGraph::GetEdge() const
{
	return _edges;
}

GameObject* AreaTakenGraph::GetBottomHexagon() const
{
	return _bottomHexagon;
}