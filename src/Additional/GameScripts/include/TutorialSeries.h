#pragma once 

#include <core/GameObject.h>
#include <manager/SceneManager.h>
#include <vector>
#include <tools/YamlConverters.h>
#include <core/Time.h>
#include <core/AudioComponent.h>
#include <GameManager.h>

using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;


class TutorialSeries : public Component {
	public:
		AudioComponent* audioComponent = nullptr;
		int displayIndex = 0;
		std::vector<GameObject*> tutorials;
		std::vector<size_t> voiceLinesAudioIds;
		bool block = false;
		bool closeOnLast = false;
		bool performAudioOnStart = false;

		virtual void Initialize() override {
			audioComponent = (AudioComponent*)(GetTransform()->GetParent()->GetGameObject()->GetComponent<AudioComponent>());

			GetGameObject()->OnActiveChangedEvent.AddCallback([&](GameObject* go) {
				if (go->GetActive()) {
					//SPDLOG_INFO("TutorialsSeries OnActiveOn!");
					if (audioComponent != nullptr && voiceLinesAudioIds.size() > 0) {
						audioComponent->SetAudio(voiceLinesAudioIds[0]);
						audioComponent->Play();
					}
				}
				});

			if (GetGameObject()->GetActive() && audioComponent != nullptr && voiceLinesAudioIds.size() > 0) {
				performAudioOnStart = true;
			}
		}

		virtual void Update() override {
			Time::_timeMultiplier = 0.0f;
			//GameManager::instance->gameStartUp = false;
			if (performAudioOnStart) {
				audioComponent->SetAudio(voiceLinesAudioIds[0]);
				audioComponent->Play();
				performAudioOnStart = false;
			}
			if (Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::RIGHT)) {
				if (!block) {
					if (tutorials.size() > 0) {
						bool playAudio = audioComponent != nullptr;
						if (playAudio && displayIndex < voiceLinesAudioIds.size()) {
							audioComponent->Stop();
						}
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
							if (playAudio && displayIndex < voiceLinesAudioIds.size()) {
								audioComponent->SetAudio(voiceLinesAudioIds[displayIndex]);
								audioComponent->Play();
							}

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

		virtual void OnDestroy() override {
			Time::_timeMultiplier = 1.0f;
		}

		virtual YAML::Node Serialize() const override {
			YAML::Node node = Component::Serialize();
			node["type"] = "TutorialSeries";

			node["tutorialsIds"] = std::vector<size_t>();
			node["closeOnLast"] = closeOnLast;
			for (size_t i = 0; i < tutorials.size(); ++i) {
				node["tutorialsIds"].push_back(tutorials[i]->Id());
			}

			if (voiceLinesAudioIds.size() > 0) {
				node["tutorialsVoiceIds"] = std::vector<size_t>();
				for (size_t i = 0; i < voiceLinesAudioIds.size(); ++i) {
					node["tutorialsVoiceIds"].push_back(SceneManager::GetAudioSaveIdx(voiceLinesAudioIds[i]));
				}
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

			if (node["tutorialsVoiceIds"]) {
				std::vector<size_t> tutorialsVoiceIds = node["tutorialsVoiceIds"].as<std::vector<size_t>>();
				for (size_t i = 0; i < tutorialsVoiceIds.size(); ++i) {
					voiceLinesAudioIds.push_back(SceneManager::GetAudio(tutorialsVoiceIds[i]));
				}
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
				ImGui::DragInt(string("CurrentIndex##").append(id).c_str(), &index, 0, tutorials.size());
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