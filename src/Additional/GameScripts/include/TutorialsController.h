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
		bool RadioStationTutUsed = true;
		bool ConcertRoadTutUsed = true;
		GameObject* AlbumTutorial = nullptr;
		GameObject* RadioStationTutorial = nullptr;
		GameObject* ConcertRoadTutorial = nullptr;
		float sqrDistanceToTrigerTut = 1.1f;
		std::vector<glm::vec3> radiostadionPosition;
		bool abilitiesTriggered = false;
		glm::vec3 startUpPlayerPos;

		static TutorialsController* instance;

		virtual void Initialize() override {

			if (instance == nullptr) 
			{
				instance = this;
				RadioStationTutorial = SceneManager::FindObjectByName("RadioStationTutorial");
				ConcertRoadTutorial = SceneManager::FindObjectByName("ConcertRoadTut");
				AlbumTutorial = SceneManager::FindObjectByName("AlbumTutorial");
				
				GetGameObject()->OnActiveChangedEvent.AddCallback([&](GameObject* go) {
						if (go->GetActive()) {
							startUpPlayerPos = GameManager::instance->GetPlayer()->GetTransform()->GetGlobalPosition();
						}
					});

				for (auto& rs : SceneManager::GetComponentsOfType<RadioStation>()) {
					radiostadionPosition.push_back(rs.second->GetTransform()->GetGlobalPosition());
				}
			}
			else 
			{
				SceneManager::DestroyGameObject(GetGameObject());
			}
		}

		virtual void Update() override {
			glm::vec3 playerPos = GameManager::instance->GetPlayer()->GetTransform()->GetGlobalPosition();
			glm::vec3 distance(0.0f);

			if (!abilitiesTriggered) {
				distance = startUpPlayerPos - playerPos;
				if (glm::dot(distance, distance) >= 0.5) {
					AlbumTutorial->SetActive(true);
					abilitiesTriggered = true;
					RadioStationTutUsed = false;
					ConcertRoadTutUsed = false;
				}
			}

			if (!ConcertRoadTutUsed && (!RadioStationTutorial->GetActive()) && (!AlbumTutorial->GetActive())) {
				for (auto& rmp : ConcertRoad::instance->RoadMapPoints) {
					distance = rmp.hexTile->GetTransform()->GetGlobalPosition() - playerPos;
					if (glm::dot(distance, distance) <= sqrDistanceToTrigerTut) {
						ConcertRoadTutorial->SetActive(true);
						ConcertRoadTutUsed = true;
					}
				}
			}

			if (!RadioStationTutUsed && (!ConcertRoadTutorial->GetActive()) && (!AlbumTutorial->GetActive())) {
				for (auto& rs : radiostadionPosition) {
					distance = rs - playerPos;
					if (glm::dot(distance, distance) <= sqrDistanceToTrigerTut) {
						RadioStationTutorial->SetActive(true);
						RadioStationTutUsed = true;
					}
				}
			}

			if (ConcertRoadTutUsed && RadioStationTutUsed && abilitiesTriggered) {
				GetGameObject()->SetActive(false);
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