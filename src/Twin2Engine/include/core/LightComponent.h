#pragma once

#include <graphic/LightingController.h>
#include <core/Component.h>
#include <tools/EventHandler.h>

namespace Twin2Engine {
	namespace Core {

		class LightComponent : public Component {
			protected:
				virtual YAML::Node Serialize() const override {
					YAML::Node node = Component::Serialize();
					node["type"] = "LightComponent";
					return node;
				}

				virtual void DrawEditor() override {
					std::string id = std::string(std::to_string(this->GetId()));
					std::string name = std::string("Light##").append(id);
					if (ImGui::CollapsingHeader(name.c_str())) {
						ImGui::TextColored(ImVec4(0.5f, 0.4f, 0.5f, 1.f), "Nothing to edit");
					}
				}
		};
	}
}