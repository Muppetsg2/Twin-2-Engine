#pragma once

#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapRegion.h>
#include <Generation/MapHexTile.h>
#include <Generation/MapSector.h>
#include <Algorithms/ObjectsKMeans.h>
#include <Tilemap/HexagonalTilemap.h>
#include <core/GameObject.h>
#include <core/Random.h>
#include <core/Transform.h>

namespace Generation::Generators
{
    class SectorGeneratorForRegionsByKMeans : public AMapElementGenerator 
    {
        SCRIPTABLE_OBJECT_BODY(SectorGeneratorForRegionsByKMeans)

    public:
        std::string prefabPath = "";
        Twin2Engine::Core::Prefab* sectorPrefab;
        int sectorsCount = 3; // Number of regions/clusters

        bool isDiscritizedHeight = false;
        float lowerHeightRange = 0.0f;
        float upperHeightRange = 0.0f;
        float heightRangeFacor = 1.0f;

        void Generate(Tilemap::HexagonalTilemap* tilemap) override;
        void Clear() override;

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

            if (sectorPrefab != nullptr) {
                if (!prefabNames.contains(sectorPrefab->GetId())) {
                    sectorPrefab = nullptr;
                }
            }

            size_t prefabId = sectorPrefab != nullptr ? sectorPrefab->GetId() : 0;

            if (ImGui::BeginCombo(string("sectorPrefab##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

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
                        sectorPrefab = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                        prefabPath = Twin2Engine::Manager::PrefabManager::GetPrefabPath(sectorPrefab);
                    }
                    else {
                        sectorPrefab = nullptr;
                        prefabPath = "";
                    }
                }

                ImGui::EndCombo();
            }

            prefabNames.clear();

            ImGui::InputInt(string("sectorsCount##SO").append(id).c_str(), &sectorsCount);
            ImGui::Checkbox(string("isDiscritizedHeight##SO").append(id).c_str(), &isDiscritizedHeight);
            ImGui::InputFloat(string("lowerHeightRange##SO").append(id).c_str(), &lowerHeightRange);
            ImGui::InputFloat(string("upperHeightRange##SO").append(id).c_str(), &upperHeightRange);
            ImGui::InputFloat(string("heightRangeFacor##SO").append(id).c_str(), &heightRangeFacor);

            ScriptableObject::DrawInheritedFields();
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(SectorGeneratorForRegionsByKMeans, Generation::Generators)
