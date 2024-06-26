#pragma once 

#include <core/GameObject.h>
#include <vector>
#include <tools/YamlConverters.h>
#include <GameManager.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;


class BlockStartSelTut : public Component {
public:
	int eventId = -1;
	bool gameStartUp = false;
	virtual void Initialize() override {
		eventId = GetGameObject()->OnActiveChangedEvent.AddCallback([&](GameObject* go) {
			if (!go->GetActive()) {
				gameStartUp = true;
				GameManager* gm = GameManager::instance;
				if (gm) {
					gm->gameStartUp = true;
				}
			}
			});
	}

	virtual void Update() override {
		GameManager* gm = GameManager::instance;
		if (gm) {
			gm->gameStartUp = gameStartUp;
		}
	}

	virtual void OnDestroy() override {
		GameManager* gm = GameManager::instance;
		if (gm) {
			gm->gameStartUp = true;
		}
		if (eventId != -1) {
			GetGameObject()->OnActiveChangedEvent.RemoveCallback(eventId);
		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "BlockStartSelTut";

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!Component::Deserialize(node))
			return false;

		return true;
	}

#if _DEBUG
	virtual void DrawEditor() override {
		string id = string(std::to_string(this->GetId()));
		string name = string("BlockStartSelTut##Component").append(id);
		if (ImGui::CollapsingHeader(name.c_str())) {

			if (Component::DrawInheritedFields()) return;
		}
	}
#endif
};