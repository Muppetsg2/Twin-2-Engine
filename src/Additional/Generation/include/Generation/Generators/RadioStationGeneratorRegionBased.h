#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class RadioStationGeneratorRegionBased : public AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(RadioStationGeneratorRegionBased)

    public:
        //Twin2Engine::Core::GameObject* prefabRadioStation;
        Twin2Engine::Core::Prefab* prefabRadioStation;
        float densityFactorPerRegion = 1.0f;

        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;
        virtual void Clear() override;

		SO_SERIALIZE()
		SO_DESERIALIZE()

#if _DEBUG
        virtual void DrawEditor() override
        {
            string id = to_string(GetId());

            ImGui::Text("Name: ");
            ImGui::SameLine();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::Text(Twin2Engine::Manager::ScriptableObjectManager::GetName(GetId()).c_str());
            ImGui::PopFont();

            std::map<size_t, string> prefabNames = Twin2Engine::Manager::PrefabManager::GetAllPrefabsNames();

            prefabNames.insert(std::pair(0, "None"));

            if (prefabRadioStation != nullptr) {
                if (!prefabNames.contains(prefabRadioStation->GetId())) {
                    prefabRadioStation = nullptr;
                }
            }

            size_t prefabId = prefabRadioStation != nullptr ? prefabRadioStation->GetId() : 0;

            if (ImGui::BeginCombo(string("prefabRadioStation##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

                bool clicked = false;
                size_t choosed = prefabId;
                for (auto& item : prefabNames) {

                    if (ImGui::Selectable(item.second.append("##").append(id).c_str(), item.first == prefabId)) {

                        if (clicked) continue;

                        choosed = item.first;
                        clicked = true;
                    }
                }

                if (clicked) {
                    if (choosed != 0) {
                        prefabRadioStation = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                    }
                    else {
                        prefabRadioStation = nullptr;
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::InputFloat(string("densityFactorPerRegion##SO").append(id).c_str(), &densityFactorPerRegion);

            ScriptableObject::DrawInheritedFields();
        }
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(RadioStationGeneratorRegionBased, Generation::Generators)