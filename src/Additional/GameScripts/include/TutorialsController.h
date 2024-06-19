#pragma once 

#include <core/GameObject.h>
#include <manager/SceneManager.h>
#include <vector>
#include <tools/YamlConverters.h>
#include <core/Time.h>
#include <GameManager.h>
#include <Player.h>

#include <ConcertRoad.h>


using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;

class TutorialsController : public Component {
	public:
		bool RadioStationTutUsed = false;
		bool ConcertRoadTutUsed = false;
		GameObject* RadioStationTutorial = nullptr;
		GameObject* ConcertRoadTutorial = nullptr;

		virtual void Initialize() override {
			RadioStationTut = SceneManager::FindObjectByName("RadioStationTutorial");
			ConcertRoadTut = SceneManager::FindObjectByName("ConcertRoadTut");
			ConcertRoad::instance->RoadMapPoints
		}

		virtual void Update() override {
			if (!ConcertRoadTutUsed) {
				glm::vec3 distance(0.0f);
				//glm::vec3 playerPos = GameManager::instance->GetPlayer()->GetT;
			}
		}

		virtual YAML::Node Serialize() const override {
			YAML::Node node = Component::Serialize();
			node["type"] = "TutorialsController";


			return node;
		}

		virtual bool Deserialize(const YAML::Node& node) override {
			if (!Component::Deserialize(node))
				return false;

			return true;
		}

#if	_DEBUG
		virtual void DrawEditor() override {
			string id = string(std::to_string(this->GetId()));
			string name = string("TutorialsController##Component").append(id);
			if (ImGui::CollapsingHeader(name.c_str())) {

				if (Component::DrawInheritedFields()) return;

			}
		}
#endif
};