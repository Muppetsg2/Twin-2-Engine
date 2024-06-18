#include <UIScripts/AreaTakenGraph.h>

using namespace Twin2Engine::UI;

void AreaTakenGraph::UpdateTopHexagon()
{
	Prefab* topHexagonPrefab = PrefabManager::GetPrefab(_topHexagonPrefabId);
	if (topHexagonPrefab == nullptr) return;

	/*std::vector<Playable*> entities = GameManager::instance->entities;

	if (entities.size() + 1 != _topHexagons.size()) {
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

		img->SetColor(GetColor((TILE_COLOR)(uint8_t)(entity->colorIdx == 0 ? 1 : powf(2.f, (float)(entity->colorIdx)))));
		img->SetLayer(_layer + 2);

		offset += progress;
		++idx;
	}*/

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

	img->SetColor(GetColor(TILE_COLOR::RED));
	img->SetLayer(_layer + 2);

	/*if (offset < 100.f) {
		_topHexagons[idx]->SetActive(true);
		Image* img = _topHexagons[idx]->GetComponent<Image>();
		img->SetFillOffset(offset);
		img->SetFillProgress(100.f);
		img->SetColor(GetColor(TILE_COLOR::NEUTRAL));
		img->SetLayer(_layer + 2);
	}
	else {
		_topHexagons[idx]->SetActive(false);
	}*/

	if (_takenPercentage < 100.f) {
		_topHexagons[1]->SetActive(true);
		Image* img = _topHexagons[1]->GetComponent<Image>();
		img->SetFillOffset(_takenPercentage);
		img->SetFillProgress(100.f);
		img->SetColor(GetColor(TILE_COLOR::NEUTRAL));
		img->SetLayer(_layer + 2);
	}
	else {
		_topHexagons[1]->SetActive(false);
	}
}

void AreaTakenGraph::UpdateEdge()
{
	Prefab* edgePrefab = PrefabManager::GetPrefab(_edgePrefabId);
	if (edgePrefab == nullptr) return;

	/*std::vector<Playable*> entities = GameManager::instance->entities;

	if (entities.size() + 1 != _edges.size()) {
		while (_edges.size() < entities.size() + 1) {
			_edges.push_back(SceneManager::CreateGameObject(edgePrefab, GetTransform()));
		}

		while (_edges.size() > entities.size() + 1) {
			SceneManager::DestroyGameObject(_edges[_edges.size() - 1]);
			_edges.erase(_edges.end() - 1);
		}
	}

	std::vector<HexTile*> tiles = GameManager::instance->Tiles;
	static const float lowAlpha = 120.f;
	static const float highAlpha = 240.f;
	static const float halfAlpha = (lowAlpha + highAlpha) * 0.5f;
	
	float offset = 0.f;
	float alpha0 = 0.f;
	size_t idx = 0;
	for (auto& entity : entities) {
		alpha0 = 3.6f * offset;

		float progress = ((float)entity->OwnTiles.size() / tiles.size()) * 100.f;
		float alpha = 3.6f * progress;

		if (alpha0 + alpha > lowAlpha && alpha0 < highAlpha) {
			_edges[idx]->SetActive(true);
			Image* img = _edges[idx]->GetComponent<Image>();
			img->SetColor(GetColor((TILE_COLOR)(uint8_t)(entity->colorIdx == 0 ? 1 : powf(2.f, (float)(entity->colorIdx)))) * 0.75f);
			img->SetLayer(_layer + 1);

			// FIRST TRIANGLE
			if (alpha0 < halfAlpha) {
				// HEXAGON EDGE START POS
				static const glm::vec2 v2s = { glm::sqrt(3) * 0.5f, -0.5f };
				// HEXAGON EDGE VECTOR
				static const glm::vec2 v2 = { -glm::sqrt(3) * 0.5f, -0.5f };

				// EDGE PERCENT
				float t;
				// CIRCLE RADIUS VECTOR
				glm::vec2 v1;
				// EDGE MOVE VECTOR
				glm::vec2 v2m;
				// FINAL PERCENT
				float percent;

				if (alpha0 > lowAlpha) {
					float clampAlpha0 = glm::clamp(alpha0, lowAlpha, halfAlpha);

					v1 = { glm::sin(glm::radians(clampAlpha0)), glm::cos(glm::radians(clampAlpha0)) };
					t = v2s.x / (v1.x - v2.x);

					v2m = v2 * t;

					percent = glm::length(v2m);

					img->SetFillOffset(percent * 50.f);
				}
				else {
					img->SetFillOffset(0.f);
				}

				float clampAlpha = glm::clamp(alpha0 + alpha, lowAlpha, halfAlpha);

				v1 = { glm::sin(glm::radians(clampAlpha)), glm::cos(glm::radians(clampAlpha)) };
				t = v2s.x / (v1.x - v2.x);

				v2m = v2 * t;

				percent = glm::length(v2m);

				img->SetFillProgress(percent * 50.f);
			}

			// SECOND TRIANGLE
			if (alpha0 + alpha > halfAlpha) {

			}
		}
		else {
			_edges[idx]->SetActive(false);
		}

		offset += progress;
		++idx;
	}*/

	if (2 != _edges.size()) {
		while (_edges.size() < 2) {
			_edges.push_back(SceneManager::CreateGameObject(edgePrefab, GetTransform()));
		}

		while (_edges.size() > 2) {
			SceneManager::DestroyGameObject(_edges[_edges.size() - 1]);
			_edges.erase(_edges.end() - 1);
		}
	}

	const float R2 = GetTransform()->GetGlobalScale().y / 1.143f;
	const glm::vec2 edgeStartPoint = glm::normalize(glm::vec2{ glm::sqrt(3) * 0.5f, -R2 * 0.5f });
	const glm::vec2 edgeMiddlePoint = glm::normalize(glm::vec2{ 0, -R2 });
	const glm::vec2 edgeVector = edgeMiddlePoint - edgeStartPoint;
	const glm::vec2 topVector = { 0, 1.f };

	const float edgeA = edgeMiddlePoint.y - edgeStartPoint.y;
	const float edgeB = edgeStartPoint.x - edgeMiddlePoint.x;
	const float edgeC = -edgeStartPoint.x * edgeA - edgeStartPoint.y * edgeB;

	const float edgeStartAlpha = glm::degrees(glm::acos(topVector.x * edgeStartPoint.x + topVector.y * edgeStartPoint.y));
	const float edgeHalfAlpha = 180.f;

	float alpha0 = 0.f;
	float alpha = 3.6f * _takenPercentage;

	if (alpha0 + alpha > edgeStartAlpha && alpha0 < 240.f) {
		_edges[0]->SetActive(true);
		Image* img = _edges[0]->GetComponent<Image>();
		img->SetColor(GetColor(TILE_COLOR::RED) * 0.75f);
		img->SetLayer(_layer + 1);
		//img->SetFillOffset(glm::clamp(alpha0 * 5.f / 6.f - 100.f, 0.f, 100.f));
		//img->SetFillProgress(glm::clamp((alpha0 + alpha) * 5.f / 6.f - 100.f, 0.f, 100.f));
		//img->SetFillOffset(glm::clamp((0.f /* offset */ * 1000.f - 3230.f) / 353.f, 0.f, 100.f));
		//img->SetFillProgress(glm::clamp(((0.f /* offset */ + _takenPercentage) * 1000.f - 3230.f) / 353.f, 0.f, 100.f));
		/*img->SetFillOffset(glm::clamp(-0.0139f * alpha0 * alpha0 + 5.f * alpha0 - 400.f, 0.f, 100.f));
		img->SetFillProgress(glm::clamp(-0.0139f * (alpha0 + alpha) * (alpha0 + alpha) + 5.f * (alpha0 + alpha) - 400.f, 0.f, 100.f));*/
		// GOOD
		//img->SetFillOffset(glm::clamp(0.0016f * 0.f * 0.f + 2.7022f * 0.f - 88.995f, 0.f, 100.f));
		//img->SetFillProgress(glm::clamp(0.0016f * (0.f + _takenPercentage) * (0.f + _takenPercentage) + 2.7022f * (0.f + _takenPercentage) - 88.995f, 0.f, 100.f));
		
		// GOOD
		/*img->SetFillOffset(glm::clamp(2.8302f * 0.f - 91.509f, 0.f, 100.f));
		img->SetFillProgress(glm::clamp(2.8302f * (0.f + _takenPercentage) - 91.509f, 0.f, 100.f));*/
		
		// FIRST TRIANGLE
		if (alpha0 < edgeHalfAlpha) {
			// RADIUS VECTOR
			glm::vec2 v1;
			// MOVE VALUE
			float t;
			// EDGE MOVE VECTOR
			glm::vec2 v2m;
			// FINAL PERCENT
			float percent;

			if (alpha0 > edgeStartAlpha) {
				float clampAlpha0 = glm::clamp(alpha0, edgeStartAlpha, edgeHalfAlpha);

				v1 = { glm::sin(glm::radians(clampAlpha0)), glm::cos(glm::radians(clampAlpha0)) };

				t = -edgeC / (edgeA * v1.x + edgeB * v1.y);

				v2m = v1 * t - edgeStartPoint;

				percent = glm::length(v2m) / glm::length(edgeVector);

				img->SetFillOffset(percent * 50.f);
			}
			else {
				img->SetFillOffset(0.f);
			}

			float clampAlpha = glm::clamp(alpha0 + alpha, edgeStartAlpha, edgeHalfAlpha);

			v1 = { glm::sin(glm::radians(clampAlpha)), glm::cos(glm::radians(clampAlpha)) };

			t = -edgeC / (edgeA * v1.x + edgeB * v1.y);

			v2m = v1 * t - edgeStartPoint;

			percent = glm::length(v2m) * glm::length(edgeVector);

			img->SetFillProgress(percent * 50.f);
		}

		// SECOND TRIANGLE
		if (alpha0 + alpha > edgeHalfAlpha) {

		}
	}
	else {
		_edges[0]->SetActive(false);
	}

	/*alpha0 = 3.6f * offset;
	if (alpha0 < highAlpha) {
		_edges[idx]->SetActive(true);
		Image* img = _edges[idx]->GetComponent<Image>();
		//img->SetFillOffset(glm::clamp(alpha0 * fiveOverSix - 100.f, 0.f, 100.f));
		img->SetFillProgress(100.f);
		img->SetColor(GetColor(TILE_COLOR::NEUTRAL) * 0.75f);
		img->SetLayer(_layer + 1);
	}
	else {
		_edges[idx]->SetActive(false);
	}
	_edges[idx]->SetActive(false);*/

	alpha0 = 3.6f * _takenPercentage;
	if (alpha0 < 240.f) {
		_edges[1]->SetActive(true);
		Image* img = _edges[1]->GetComponent<Image>();
		//img->SetFillOffset(glm::clamp(alpha0 * 5.f / 6.f - 100.f, 0.f, 100.f));
		//img->SetFillOffset(glm::clamp((_takenPercentage * 1000.f - 3230.f) / 353.f, 0.f, 100.f));
		img->SetFillProgress(100.f);
		img->SetColor(GetColor(TILE_COLOR::NEUTRAL) * 0.75f);
		img->SetLayer(_layer + 1);
	}
	else {
		_edges[1]->SetActive(false);
	}
	_edges[1]->SetActive(false);
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

glm::vec4 AreaTakenGraph::GetColor(const TILE_COLOR& color)
{
	switch (color) {
	case TILE_COLOR::BLUE:
		return glm::vec4(0.31f, 0.361f, 0.886f, 1.f);
	case TILE_COLOR::CYAN:
		return glm::vec4(0.31f, 0.867f, 0.886f, 1.f);
	case TILE_COLOR::GREEN:
		return glm::vec4(0.31f, 0.886f, 0.396f, 1.f);
	case TILE_COLOR::PINK:
		return glm::vec4(0.894f, 0.31f, 0.686f, 1.f);
	case TILE_COLOR::PURPLE:
		return glm::vec4(0.769f, 0.31f, 0.89f, 1.f);
	case TILE_COLOR::RED:
		return glm::vec4(0.886f, 0.406f, 0.31f, 1.f);
	case TILE_COLOR::YELLOW:
		return glm::vec4(0.875f, 0.886f, 0.31f, 1.f);
	case TILE_COLOR::NEUTRAL:
	default:
		return glm::vec4(1.f);
	}
}

void AreaTakenGraph::Update()
{
	if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
		/*UpdateTopHexagon();
		UpdateEdge();
		UpdateBottomHexagon();*/
	}
	UpdateTopHexagon();
	UpdateEdge();
	UpdateBottomHexagon();
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
	node["layer"] = _layer;
	node["topCirclePrefab"] = SceneManager::GetPrefabSaveIdx(_topHexagonPrefabId);
	node["edgePrefab"] = SceneManager::GetPrefabSaveIdx(_edgePrefabId);
	node["bottomCirclePrefab"] = SceneManager::GetPrefabSaveIdx(_bottomHexagonPrefabId);
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

	if (node["topCirclePrefab"]) {
		_topHexagonPrefabId = SceneManager::GetPrefab(node["topCirclePrefab"].as<size_t>());
	}
	isGood = isGood && node["topCirclePrefab"];

	if (node["edgePrefab"]) {
		_edgePrefabId = SceneManager::GetPrefab(node["edgePrefab"].as<size_t>());
	}
	isGood = isGood && node["edgePrefab"];

	if (node["bottomCirclePrefab"]) {
		_bottomHexagonPrefabId = SceneManager::GetPrefab(node["bottomCirclePrefab"].as<size_t>());
	}
	isGood = isGood && node["bottomCirclePrefab"];

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

		prefabId = _bottomHexagonPrefabId;
		if (PrefabDropDown(string("Bottom Hexagon Prefab##").append(id).c_str(), &prefabId, id)) {
			SetBottomHexagonPrefabId(prefabId);
		}

		ImGui::DragFloat(string("Taken Percentage##").append(id).c_str(), &_takenPercentage);
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

void AreaTakenGraph::SetBottomHexagonPrefabId(size_t prefabId)
{
	if (_bottomHexagonPrefabId != prefabId) {
		_bottomHexagonPrefabId = prefabId;
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