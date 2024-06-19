#include <UIScripts/AreaTakenGraph.h>

using namespace Twin2Engine::UI;

#define AREA_TAKEN_GRAPH_TEST false;

void AreaTakenGraph::UpdateTopHexagon()
{
	Prefab* topHexagonPrefab = PrefabManager::GetPrefab(_topHexagonPrefabId);
	if (topHexagonPrefab == nullptr) return;

#if !AREA_TAKEN_GRAPH_TEST
	const std::vector<Playable*>& entities = GameManager::instance->entities;

	if (entities.size() + 1 != _topHexagons.size()) {
		while (_topHexagons.size() < entities.size() + 1) {
			_topHexagons.push_back(SceneManager::CreateGameObject(topHexagonPrefab, GetTransform()));
		}

		while (_topHexagons.size() > entities.size() + 1) {
			SceneManager::DestroyGameObject(_topHexagons[_topHexagons.size() - 1]);
			_topHexagons.erase(_topHexagons.end() - 1);
		}
	}

	const size_t tilesNum = GameManager::instance->Tiles.size();
	float offset = 0.f;
	size_t idx = 0;
	for (auto& entity : entities) {
		float progress = ((float)entity->OwnTiles.size() / tilesNum) * 100.f;

		Image* img = _topHexagons[idx]->GetComponent<Image>();
		img->SetFillOffset(offset);
		img->SetFillProgress(offset + progress);

		img->SetColor({ GetColor((TILE_COLOR)(uint8_t)(entity->colorIdx == 0 ? 1 : powf(2.f, (float)(entity->colorIdx)))), 1.f });
		img->SetLayer(_layer + 1);

		offset += progress;
		++idx;
	}

	if (offset < 100.f) {
		_topHexagons[idx]->SetActive(true);
		Image* img = _topHexagons[idx]->GetComponent<Image>();
		img->SetFillOffset(offset);
		img->SetFillProgress(100.f);
		img->SetColor({ GetColor(TILE_COLOR::NEUTRAL), 1.f });
		img->SetLayer(_layer + 1);
	}
	else {
		_topHexagons[idx]->SetActive(false);
	}

#else
	if (2 != _topHexagons.size()) {
		while (_topHexagons.size() < 2) {
			_topHexagons.push_back(SceneManager::CreateGameObject(topHexagonPrefab, GetTransform()));
}

		while (_topHexagons.size() > 2) {
			SceneManager::DestroyGameObject(_topHexagons[_topHexagons.size() - 1]);
			_topHexagons.erase(_topHexagons.end() - 1);
		}
	}

	Image* img = _topHexagons[0]->GetComponent<Image>();
	img->SetFillOffset(0.f);
	img->SetFillProgress(_takenPercentage);

	img->SetColor({ GetColor(TILE_COLOR::RED), 1.f });
	img->SetLayer(_layer + 1);

	if (_takenPercentage < 100.f) {
		_topHexagons[1]->SetActive(true);
		Image* img = _topHexagons[1]->GetComponent<Image>();
		img->SetFillOffset(_takenPercentage);
		img->SetFillProgress(100.f);
		img->SetColor({ GetColor(TILE_COLOR::NEUTRAL), 1.f });
		img->SetLayer(_layer + 1);
	}
	else {
		_topHexagons[1]->SetActive(false);
	}
#endif
}

void AreaTakenGraph::UpdateEdge()
{
	Prefab* edgePrefab = PrefabManager::GetPrefab(_edgePrefabId);
	if (edgePrefab == nullptr) return;

	const float R2 = GetTransform()->GetGlobalScale().y / 1.143f;
	const glm::vec2 edgeStartPoint = { glm::sqrt(3.f) * 0.5f, -R2 * 0.5f };
	const glm::vec2 edgeMiddlePoint = { 0.f, -R2 };
	const glm::vec2 edgeEndPoint = { -glm::sqrt(3.f) * 0.5f, -R2 * 0.5f };
	const glm::vec2 rightEdgeVector = edgeMiddlePoint - edgeStartPoint;
	const glm::vec2 leftEdgeVector = edgeEndPoint - edgeMiddlePoint;
	const glm::vec2 topVector = { 0, 1.f };

	const float rightEdgeA = edgeMiddlePoint.y - edgeStartPoint.y;
	const float rightEdgeB = edgeStartPoint.x - edgeMiddlePoint.x;
	const float rightEdgeC = -edgeStartPoint.x * rightEdgeA - edgeStartPoint.y * rightEdgeB;

	Func<float, float> CalcRightPercent = [&](float alpha) -> float {
		// RADIUS VECTOR
		glm::vec2 v1 = { glm::sin(glm::radians(alpha)), glm::cos(glm::radians(alpha)) };

		// MOVE VALUE
		float t = -rightEdgeC / (rightEdgeA * v1.x + rightEdgeB * v1.y);

		// EDGE MOVE VECTOR
		glm::vec2 v2m = v1 * t;

		// FINAL PERCENT
		return glm::abs(v2m.x - edgeStartPoint.x) / glm::abs(rightEdgeVector.x);
		};

	const float leftEdgeA = edgeEndPoint.y - edgeMiddlePoint.y;
	const float leftEdgeB = edgeMiddlePoint.x - edgeEndPoint.x;
	const float leftEdgeC = -edgeEndPoint.x * leftEdgeA - edgeEndPoint.y * leftEdgeB;

	Func<float, float> CalcLeftPercent = [&](float alpha) -> float {
		// RADIUS VECTOR
		glm::vec2 v1 = { glm::sin(glm::radians(alpha)), glm::cos(glm::radians(alpha)) };

		// MOVE VALUE
		float t = -leftEdgeC / (leftEdgeA * v1.x + leftEdgeB * v1.y);

		// EDGE MOVE VECTOR
		glm::vec2 v2m = v1 * t;

		// FINAL PERCENT
		return glm::abs(v2m.x - edgeMiddlePoint.x) / glm::abs(leftEdgeVector.x);
		};

	const float edgeStartAlpha = glm::degrees(glm::acos(topVector.x * edgeStartPoint.x + topVector.y * edgeStartPoint.y));
	const float edgeHalfAlpha = 180.f;
	const float edgeEndAlpha = 360.f - glm::degrees(glm::acos(topVector.x * edgeEndPoint.x + topVector.y * edgeEndPoint.y));

#if !AREA_TAKEN_GRAPH_TEST
	const std::vector<Playable*>& entities = GameManager::instance->entities;

	if (entities.size() + 1 != _edges.size()) {
		while (_edges.size() < entities.size() + 1) {
			_edges.push_back(SceneManager::CreateGameObject(edgePrefab, GetTransform()));
		}

		while (_edges.size() > entities.size() + 1) {
			SceneManager::DestroyGameObject(_edges[_edges.size() - 1]);
			_edges.erase(_edges.end() - 1);
		}
	}

	const size_t tilesNum = GameManager::instance->Tiles.size();
	
	float offset = 0.f;
	float alpha0 = 0.f;
	size_t idx = 0;
	for (auto& entity : entities) {
		alpha0 = 3.6f * offset;

		float progress = ((float)entity->OwnTiles.size() / tilesNum) * 100.f;
		float alpha = 3.6f * progress;

		if (alpha0 + alpha > edgeStartAlpha && alpha0 < edgeEndAlpha) {
			_edges[idx]->SetActive(true);
			Image* img = _edges[idx]->GetComponent<Image>();
			img->SetColor({ GetColor((TILE_COLOR)(uint8_t)(entity->colorIdx == 0 ? 1 : powf(2.f, (float)(entity->colorIdx)))) * 0.75f, 1.f });
			img->SetLayer(_layer);

			// FIRST TRIANGLE
			if (alpha0 < edgeHalfAlpha) {
				if (alpha0 > edgeStartAlpha) {
					float clampAlpha0 = glm::clamp(alpha0, edgeStartAlpha, edgeHalfAlpha);

					img->SetFillOffset(CalcRightPercent(clampAlpha0) * 50.f);
				}
				else {
					img->SetFillOffset(0.f);
				}

				float clampAlpha = glm::clamp(alpha0 + alpha, edgeStartAlpha, edgeHalfAlpha);

				img->SetFillProgress(CalcRightPercent(clampAlpha) * 50.f);
			}
			// SECOND TRIANGLE
			else {
				float clampAlpha0 = glm::clamp(alpha0, edgeHalfAlpha, edgeEndAlpha);

				img->SetFillOffset(CalcLeftPercent(clampAlpha0) * 50.f + 50.f);
			}

			if (alpha0 + alpha > edgeHalfAlpha) {
				float clampAlpha = glm::clamp(alpha0 + alpha, edgeHalfAlpha, edgeEndAlpha);

				img->SetFillProgress(CalcLeftPercent(clampAlpha) * 50.f + 50.f);
			}
		}
		else {
			_edges[idx]->SetActive(false);
		}

		offset += progress;
		++idx;
	}

	alpha0 = 3.6f * offset;
	if (alpha0 < edgeEndAlpha) {
		_edges[idx]->SetActive(true);
		Image* img = _edges[idx]->GetComponent<Image>();
		
		if (alpha0 < edgeHalfAlpha) {
			if (alpha0 > edgeStartAlpha) {
				float clampAlpha0 = glm::clamp(alpha0, edgeStartAlpha, edgeHalfAlpha);

				img->SetFillOffset(CalcRightPercent(clampAlpha0) * 50.f);
			}
			else {
				img->SetFillOffset(0.f);
			}
		}
		else {
			float clampAlpha0 = glm::clamp(alpha0, edgeHalfAlpha, edgeEndAlpha);

			img->SetFillOffset(CalcLeftPercent(clampAlpha0) * 50.f + 50.f);
		}

		img->SetFillProgress(100.f);
		img->SetColor({ GetColor(TILE_COLOR::NEUTRAL) * 0.75f, 1.f });
		img->SetLayer(_layer);
	}
	else {
		_edges[idx]->SetActive(false);
	}

#else

	if (2 != _edges.size()) {
		while (_edges.size() < 2) {
			_edges.push_back(SceneManager::CreateGameObject(edgePrefab, GetTransform()));
		}

		while (_edges.size() > 2) {
			SceneManager::DestroyGameObject(_edges[_edges.size() - 1]);
			_edges.erase(_edges.end() - 1);
		}
	}

	float alpha0 = 0.f;
	float alpha = 3.6f * _takenPercentage;

	if (alpha0 + alpha > edgeStartAlpha && alpha0 < edgeEndAlpha) {
		_edges[0]->SetActive(true);
		Image* img = _edges[0]->GetComponent<Image>();
		img->SetColor({ GetColor(TILE_COLOR::RED) * 0.75f, 1.f });
		img->SetLayer(_layer);

		// FIRST TRIANGLE
		if (alpha0 < edgeHalfAlpha) {
			if (alpha0 > edgeStartAlpha) {
				float clampAlpha0 = glm::clamp(alpha0, edgeStartAlpha, edgeHalfAlpha);

				img->SetFillOffset(CalcRightPercent(clampAlpha0) * 50.f);
			}
			else {
				img->SetFillOffset(0.f);
			}

			float clampAlpha = glm::clamp(alpha0 + alpha, edgeStartAlpha, edgeHalfAlpha);

			img->SetFillProgress(CalcRightPercent(clampAlpha) * 50.f);
		}
		// SECOND TRIANGLE
		else {
			float clampAlpha0 = glm::clamp(alpha0, edgeHalfAlpha, edgeEndAlpha);

			img->SetFillOffset(CalcLeftPercent(clampAlpha0) * 50.f + 50.f);
		}

		if (alpha0 + alpha > edgeHalfAlpha) {
			float clampAlpha = glm::clamp(alpha0 + alpha, edgeHalfAlpha, edgeEndAlpha);

			img->SetFillProgress(CalcLeftPercent(clampAlpha) * 50.f + 50.f);
		}
	}
	else {
		_edges[0]->SetActive(false);
	}

	alpha0 = 3.6f * _takenPercentage;
	if (alpha0 < edgeEndAlpha) {
		_edges[1]->SetActive(true);
		Image* img = _edges[1]->GetComponent<Image>();

		if (alpha0 < edgeHalfAlpha) {
			if (alpha0 > edgeStartAlpha) {
				float clampAlpha0 = glm::clamp(alpha0, edgeStartAlpha, edgeHalfAlpha);

				img->SetFillOffset(CalcRightPercent(clampAlpha0) * 50.f);
			}
			else {
				img->SetFillOffset(0.f);
			}
		}
		else {
			float clampAlpha0 = glm::clamp(alpha0, edgeHalfAlpha, edgeEndAlpha);

			img->SetFillOffset(CalcLeftPercent(clampAlpha0) * 50.f + 50.f);
		}

		img->SetFillProgress(100.f);
		img->SetColor({ GetColor(TILE_COLOR::NEUTRAL) * 0.75f, 1.f });
		img->SetLayer(_layer);
	}
	else {
		_edges[1]->SetActive(false);
	}

#endif
}

void AreaTakenGraph::UpdateTopValueHexagon()
{
	Prefab* topValuePrefab = PrefabManager::GetPrefab(_topValueHexagonPrefabId);
	if (topValuePrefab == nullptr) return;

	if (_topValueHexagon == nullptr) {
		_topValueHexagon = SceneManager::CreateGameObject(topValuePrefab, GetTransform());
	}
	
	if (_topHexagons.size() <= 1) {
		_topValueHexagon->SetActive(false);
		return;
	}

	Image* img = _topHexagons[0]->GetComponent<Image>();
	for (size_t i = 1; i < _topHexagons.size() - 1; ++i) {
		Image* temp = _topHexagons[i]->GetComponent<Image>();
		if (temp->GetFillProgress() - temp->GetFillOffset() > img->GetFillProgress() - img->GetFillOffset()) {
			img = temp;
		}
	}

	Image* topImg = _topValueHexagon->GetComponent<Image>();
	topImg->SetLayer(_layer + 2);
	topImg->SetColor({ glm::vec3(0.f), 1.f });
	topImg->SetFillOffset(img->GetFillOffset());
	topImg->SetFillProgress(img->GetFillProgress());
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

glm::vec3 AreaTakenGraph::GetColor(const TILE_COLOR& color)
{
	switch (color) {
	case TILE_COLOR::BLUE:
		return glm::vec3(0.31f, 0.361f, 0.886f);
	case TILE_COLOR::CYAN:
		return glm::vec3(0.31f, 0.867f, 0.886f);
	case TILE_COLOR::GREEN:
		return glm::vec3(0.31f, 0.886f, 0.396f);
	case TILE_COLOR::PINK:
		return glm::vec3(0.894f, 0.31f, 0.686f);
	case TILE_COLOR::PURPLE:
		return glm::vec3(0.769f, 0.31f, 0.89f);
	case TILE_COLOR::RED:
		return glm::vec3(0.886f, 0.406f, 0.31f);
	case TILE_COLOR::YELLOW:
		return glm::vec3(0.875f, 0.886f, 0.31f);
	case TILE_COLOR::NEUTRAL:
	default:
		return glm::vec3(1.f);
	}
}

void AreaTakenGraph::Update()
{
#if !AREA_TAKEN_GRAPH_TEST
	if (GameManager::instance != nullptr) {
		if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
			UpdateTopHexagon();
			UpdateEdge();
			UpdateTopValueHexagon();
		}
	}
#else
	UpdateTopHexagon();
	UpdateEdge();
	UpdateTopValueHexagon();
#endif
}

void AreaTakenGraph::OnDestroy()
{
	_topHexagons.clear();
	_edges.clear();
	_topValueHexagon = nullptr;
}

YAML::Node AreaTakenGraph::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "AreaTakenGraph";
	node["layer"] = _layer;
	node["topHexagonPrefab"] = SceneManager::GetPrefabSaveIdx(_topHexagonPrefabId);
	node["edgePrefab"] = SceneManager::GetPrefabSaveIdx(_edgePrefabId);
	node["topValueHexagonPrefab"] = SceneManager::GetPrefabSaveIdx(_topValueHexagonPrefabId);
	return node;
}

bool AreaTakenGraph::Deserialize(const YAML::Node& node)
{
	bool isGood = true;

	isGood = isGood && !Component::Deserialize(node);

	if (node["layer"]) {
		_layer = node["layer"].as<uint32_t>();
	}
	isGood = isGood && node["layer"];

	if (node["topHexagonPrefab"]) {
		_topHexagonPrefabId = SceneManager::GetPrefab(node["topHexagonPrefab"].as<size_t>());
	}
	isGood = isGood && node["topHexagonPrefab"];

	if (node["edgePrefab"]) {
		_edgePrefabId = SceneManager::GetPrefab(node["edgePrefab"].as<size_t>());
	}
	isGood = isGood && node["edgePrefab"];

	if (node["topValueHexagonPrefab"]) {
		_topValueHexagonPrefabId = SceneManager::GetPrefab(node["topValueHexagonPrefab"].as<size_t>());
	}
	isGood = isGood && node["topValueHexagonPrefab"];

	return isGood;
}

#if _DEBUG
void AreaTakenGraph::DrawEditor() {
	string id = string(std::to_string(this->GetId()));
	string name = string("AreaTakenGraph##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		DrawInheritedFields();

		int32_t layer = _layer;
		if (ImGui::InputInt(string("Layer##").append(id).c_str(), &layer)) {
			SetLayer(layer);
		}

		size_t prefabId = _topHexagonPrefabId;
		if (PrefabDropDown(string("Top Hexagon Prefab##").append(id).c_str(), &prefabId, id)) {
			SetTopHexagonPrefabId(prefabId);
		}

		prefabId = _edgePrefabId;
		if (PrefabDropDown(string("Edge Prefab##").append(id).c_str(), &prefabId, id)) {
			SetEdgePrefabId(prefabId);
		}

		prefabId = _topValueHexagonPrefabId;
		if (PrefabDropDown(string("Top Value Hexagon Prefab##").append(id).c_str(), &prefabId, id)) {
			SetTopValueHexagonPrefabId(prefabId);
		}

#if AREA_TAKEN_GRAPH_TEST
		ImGui::DragFloat(string("Taken Percentage##").append(id).c_str(), &_takenPercentage);
#endif
	}
}
#endif

void AreaTakenGraph::SetLayer(int32_t layer)
{
	if (_layer != layer) {
		_layer = layer;
	}
}

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

void AreaTakenGraph::SetTopValueHexagonPrefabId(size_t prefabId)
{
	if (_topValueHexagonPrefabId != prefabId) {
		_topValueHexagonPrefabId = prefabId;
	}
}

int32_t AreaTakenGraph::GetLayer() const
{
	return _layer;
}

size_t AreaTakenGraph::GetTopHexagonPrefabId() const
{
	return _topHexagonPrefabId;
}

size_t AreaTakenGraph::GetEdgePrefabId() const
{
	return _edgePrefabId;
}

size_t AreaTakenGraph::GetTopValueHexagonPrefabId() const
{
	return _topValueHexagonPrefabId;
}

std::vector<GameObject*> AreaTakenGraph::GetTopHexagons() const
{
	return _topHexagons;
}

std::vector<GameObject*> AreaTakenGraph::GetEdge() const
{
	return _edges;
}

GameObject* AreaTakenGraph::GetTopValueHexagon() const
{
	return _topValueHexagon;
}