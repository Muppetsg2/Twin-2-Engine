#pragma once 

#include <core/GameObject.h>
#include <manager/SceneManager.h>
#include <vector>
#include <tools/YamlConverters.h>
#include <core/Time.h>
#include <GameManager.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;


class TutorialSeries : public Component {
	public:
		int displayIndex = 0;
		std::vector<GameObject*> tutorials;
		bool block = false;
		bool closeOnLast = false;

		virtual void Initialize() override {

		}

		virtual void Update() override {
			Time::_timeMultiplier = 0.0f;
			//GameManager::instance->gameStartUp = false;
			if (Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::RIGHT)) {
				if (!block) {
					if (tutorials.size() > 0) {
						tutorials[displayIndex++]->SetActive(false);
						if (displayIndex == tutorials.size()) {
							Time::_timeMultiplier = 1.0f;
							//GameManager::instance->gameStartUp = true;
							tutorials[0]->SetActive(true);
							displayIndex = 0;
							GetGameObject()->SetActive(false);
						}
						else {
							tutorials[displayIndex]->SetActive(true);

							if (displayIndex == (tutorials.size() - 1)) {
								TutorialSeries* t = tutorials[displayIndex]->GetComponent<TutorialSeries>();
								if (t != nullptr) {
									t->block = true;
									Time::_timeMultiplier = 1.0f;
									//GameManager::instance->gameStartUp = true;
									tutorials[0]->SetActive(true);
									displayIndex = 0;
									GetGameObject()->SetActive(false);
								}
								else if (closeOnLast) {
									Time::_timeMultiplier = 1.0f;
									//GameManager::instance->gameStartUp = true;
									tutorials[0]->SetActive(true);
									displayIndex = 0;
									GetGameObject()->SetActive(false);
								}
							}
						}
					}
					else {
						Time::_timeMultiplier = 1.0f;
						//GameManager::instance->gameStartUp = true;
						GetGameObject()->SetActive(false);
						//if (displayIndex == 1) {
						//	displayIndex = 0;
						//	GetGameObject()->SetActive(false);
						//}
						//else {
						//	displayIndex = 1;
						//}
					}
				}
				else {
					block = false;
				}
			}
		}

		virtual YAML::Node Serialize() const override {
			YAML::Node node = Component::Serialize();
			node["type"] = "TutorialSeries";

			node["tutorialsIds"] = std::vector<size_t>();
			node["closeOnLast"] = closeOnLast;
			for (size_t i = 0; i < tutorials.size(); ++i) {
				node["tutorialsIds"].push_back(tutorials[i]->Id());
			}

			return node;
		}

		virtual bool Deserialize(const YAML::Node& node) override {
			if (!Component::Deserialize(node))
				return false;

			std::vector<size_t> tutorialsIds = node["tutorialsIds"].as<std::vector<size_t>>();
			for (size_t i = 0; i < tutorialsIds.size(); ++i) {
				tutorials.push_back(SceneManager::GetGameObjectWithId(tutorialsIds[i]));
			}

			if (node["closeOnLast"]) {
				closeOnLast = node["closeOnLast"].as<bool>();
			}

			return true;
		}

#if _DEBUG
		virtual void DrawEditor() override {
			string id = string(std::to_string(this->GetId()));
			string name = string("TutorialSeries##Component").append(id);
			if (ImGui::CollapsingHeader(name.c_str())) {

				if (Component::DrawInheritedFields()) return;

				int index = displayIndex;
				ImGui::DragInt(string("startY##").append(id).c_str(), &index, 0, tutorials.size());
				if (displayIndex != tutorials.size()) {
					tutorials[displayIndex]->SetActive(false);
				}
				if (index == tutorials.size() && tutorials.size() > 0) {
					tutorials[0]->SetActive(true);
				}
				else if (tutorials.size() > 0) {
					tutorials[index]->SetActive(true);
				}
			}
		}
#endif
};