#pragma once

#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/MapHexTile.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class MountainsGenerator : public AMapElementGenerator
    {
        SCRIPTABLE_OBJECT_BODY(MountainsGenerator)

    public:
        //Twin2Engine::Core::GameObject* prefabMountains;
        Twin2Engine::Core::Prefab* prefabMountains;
        int mountainsNumber = 0;

        float mountainsHeight = 0.2f;

        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

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

            if (prefabMountains != nullptr) {
                if (!prefabNames.contains(prefabMountains->GetId())) {
                    prefabMountains = nullptr;
                }
            }

            size_t prefabId = prefabMountains != nullptr ? prefabMountains->GetId() : 0;

            if (ImGui::BeginCombo(string("prefabMountains##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

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
                        prefabMountains = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                    }
                    else {
                        prefabMountains = nullptr;
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::InputInt(string("mountainsNumber##SO").append(id).c_str(), &mountainsNumber);
            ImGui::InputFloat(string("mountainsHeight##SO").append(id).c_str(), &mountainsHeight);

            ScriptableObject::DrawInheritedFields();
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(MountainsGenerator, Generation::Generators)