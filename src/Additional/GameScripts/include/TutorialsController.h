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
		//bool ConcertRoadTutUsed = true;
		GameObject* AlbumTutorial = nullptr;
		GameObject* MeetingTutorial = nullptr;
		GameObject* ConcertTutorial = nullptr;
		GameObject* AlbumTut = nullptr;
		GameObject* MeetingTut = nullptr;
		GameObject* ConcertTut = nullptr;
		GameObject* ConcertTut2 = nullptr;
		GameObject* RadioStationTutorial = nullptr;
		//GameObject* ConcertRoadTutorial = nullptr;
		float sqrDistanceToTrigerTut = 1.1f;
		std::vector<glm::vec3> radiostadionPosition;
		bool abilitiesTriggered = false;
		bool abilitiesTutComplited = false;
		glm::vec3 startUpPlayerPos;

		static TutorialsController* instance;

		virtual void Initialize() override {

			if (instance == nullptr) 
			{
				instance = this;
				RadioStationTutorial = SceneManager::FindObjectByName("RadioStationTutorial");
				//ConcertRoadTutorial = SceneManager::FindObjectByName("ConcertRoadTut");
				AlbumTutorial = SceneManager::FindObjectByName("AlbumTutorial");
				MeetingTutorial = SceneManager::FindObjectByName("MeetingTutorial");
				ConcertTutorial = SceneManager::FindObjectByName("ConcertTutorial");
				AlbumTut = SceneManager::FindObjectByName("AlbumTut");
				MeetingTut = SceneManager::FindObjectByName("MeetingTut");
				ConcertTut = SceneManager::FindObjectByName("ConcertTut");
				ConcertTut2 = SceneManager::FindObjectByName("ConcertTut2");
				
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
					//RadioStationTutUsed = false;
					//ConcertRoadTutUsed = false;
				}
			}

			if (abilitiesTriggered && !abilitiesTutComplited) {
				abilitiesTutComplited = !(AlbumTutorial->GetActive() ||
										  MeetingTutorial->GetActive() ||
										  ConcertTutorial->GetActive() ||
										  AlbumTut->GetActive() ||
										  MeetingTut->GetActive() ||
										  ConcertTut->GetActive() ||
										  ConcertTut2->GetActive());
			}
			//if (!ConcertRoadTutUsed && (!RadioStationTutorial->GetActive()) && (!AlbumTutorial->GetActive())) {
			//	for (auto& rmp : ConcertRoad::instance->RoadMapPoints) {
			//		distance = rmp->GetTransform()->GetGlobalPosition() - playerPos;
			//		if (glm::dot(distance, distance) <= sqrDistanceToTrigerTut) {
			//			ConcertRoadTutorial->SetActive(true);
			//			ConcertRoadTutUsed = true;
			//		}
			//	}
			//}

			//if (!RadioStationTutUsed && (!ConcertRoadTutorial->GetActive()) && (!AlbumTutorial->GetActive())) {
			if (!RadioStationTutUsed && abilitiesTutComplited) {
				for (auto& rs : radiostadionPosition) {
					distance = rs - playerPos;
					if (glm::dot(distance, distance) <= sqrDistanceToTrigerTut) {
						RadioStationTutorial->SetActive(true);
						RadioStationTutUsed = true;
					}
				}
			}

			//if (ConcertRoadTutUsed && RadioStationTutUsed && abilitiesTriggered) {
			if (RadioStationTutUsed && abilitiesTutComplited) {
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