#pragma once
#include <GameManager.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <ui/Image.h>

class AreaTakenGraph : public Twin2Engine::Core::Component {
private:
	using GameObject = Twin2Engine::Core::GameObject;

	size_t _topHexagonPrefabId;
	size_t _edgePrefabId;
	size_t _bottomHexagonPrefabId;

	std::vector<GameObject*> _topHexagons;
	std::vector<GameObject*> _edges;
	GameObject* _bottomHexagon = nullptr;

	void UpdateTopHexagon();
	void UpdateEdge();
	void UpdateBottomHexagon();

	bool PrefabDropDown(const char* label, size_t* prefabId, const std::string& objId);

public:

	void Update() override;
	void OnDestroy() override;

	YAML::Node Serialize() const override;
	bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	void DrawEditor() override;
#endif

	void SetTopHexagonPrefabId(size_t prefabId);
	void SetEdgePrefabId(size_t prefabId);
	void SetBottomHexagonPrefabId(size_t prefabId);

	size_t GetTopHexagonPrefabId() const;
	size_t GetEdgePrefabId() const;
	size_t GetBottomHexagonPrefabId() const;
	std::vector<GameObject*> GetTopHexagons() const;
	std::vector<GameObject*> GetEdge() const;
	GameObject* GetBottomHexagon() const;
};