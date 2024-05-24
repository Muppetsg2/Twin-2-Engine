#pragma once
#include <core/Component.h>

namespace Twin2Engine {
	namespace Core {

		class LightComponent : public Component {
			protected:
				virtual YAML::Node Serialize() const override {
					YAML::Node node = Component::Serialize();
					node["type"] = "LightComponent";
					return node;
				}

				virtual bool Deserialize(const YAML::Node& node) override {
					return Component::Deserialize(node);
				}

#if _DEBUG
				virtual void DrawEditor() override {
					std::string id = std::string(std::to_string(this->GetId()));
					std::string name = std::string("Light##Component").append(id);
					if (ImGui::CollapsingHeader(name.c_str())) {
						Component::DrawInheritedFields();
						ImGui::TextColored(ImVec4(0.5f, 0.4f, 0.5f, 1.f), "Nothing to edit");
					}
				}
#endif
		};
	}
}