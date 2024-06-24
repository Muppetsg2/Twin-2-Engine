#pragma once
#include <GameManager.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <ui/Image.h>
#include <ui/Mask.h>

class AreaTakenGraph : public Twin2Engine::Core::Component {
private:
	using GameObject = Twin2Engine::Core::GameObject;

	static AreaTakenGraph* _instance;

	size_t _topHexagonPrefabId = 0;
	size_t _edgePrefabId = 0;
	size_t _topEdgePrefabId = 0;
	size_t _topValueHexagonPrefabId = 0;

	std::vector<GameObject*> _topHexagons;
	std::vector<GameObject*> _edges;
	GameObject* _topEdge = nullptr;
	GameObject* _topValueHexagon = nullptr;

	uint32_t _layer = 0;

	float _takenPercentage = 0.f;

	void UpdateTopHexagon();
	void UpdateEdge();
	void UpdateTopValueHexagon();

#if _DEBUG
	bool PrefabDropDown(const char* label, size_t* prefabId, const std::string& objId);
#endif

	glm::vec3 GetColor(const TILE_COLOR& color);


public:
	static AreaTakenGraph* Instance();

	virtual void Initialize() override;
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
	void SetTopEdgePrefabId(size_t prefabId);
	void SetTopValueHexagonPrefabId(size_t prefabId);

	int32_t GetLayer() const;
	size_t GetTopHexagonPrefabId() const;
	size_t GetEdgePrefabId() const;
	size_t GetTopEdgePrefabId() const;
	size_t GetTopValueHexagonPrefabId() const;
	std::vector<GameObject*> GetTopHexagons() const;
	std::vector<GameObject*> GetEdge() const;
	GameObject* GetTopEdge() const;
	GameObject* GetTopValueHexagon() const;

	void Reset();
};