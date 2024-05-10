#pragma once

#include <LightingController.h>
#include <core/Component.h>
#include <core/EventHandler.h>

namespace Twin2Engine {
	namespace Core {

		class LightComponent : public Component {
			protected:
				LightComponent() : Component() {};

				virtual YAML::Node Serialize() const override {
					return Component::Serialize();
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