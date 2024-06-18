#pragma once
#include <GameManager.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <ui/Image.h>

// TODO: wyczyœciæ prefaby
// TODO: Œci¹gn¹æ zmiany z main-a
// TODO: Podpiêcie pod gameplay

class AreaTakenGraph : public Twin2Engine::Core::Component {
private:
	using GameObject = Twin2Engine::Core::GameObject;

	size_t _topHexagonPrefabId = 0;
	size_t _edgePrefabId = 0;
	size_t _topValueHexagonPrefabId = 0;

	std::vector<GameObject*> _topHexagons;
	std::vector<GameObject*> _edges;
	GameObject* _topValueHexagon = nullptr;

	uint32_t _layer = 0;

	float _takenPercentage = 0.f;

	void UpdateTopHexagon();
	void UpdateEdge();
	void UpdateTopValueHexagon();

	bool PrefabDropDown(const char* label, size_t* prefabId, const std::string& objId);
	glm::vec3 GetColor(const TILE_COLOR& color);

public:

	void Update() override;
	void OnDestroy() override;

	YAML::Node Serialize() const override;
	bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	void DrawEditor() override;
#endif

	void SetLayer(int32_t layer);
	void SetTopHexagonPrefabId(size_t prefabId);
	void SetEdgePrefabId(size_t prefabId);
	void SetTopValueHexagonPrefabId(size_t prefabId);

	int32_t GetLayer() const;
	size_t GetTopHexagonPrefabId() const;
	size_t GetEdgePrefabId() const;
	size_t GetTopValueHexagonPrefabId() const;
	std::vector<GameObject*> GetTopHexagons() const;
	std::vector<GameObject*> GetEdge() const;
	GameObject* GetTopValueHexagon() const;
};