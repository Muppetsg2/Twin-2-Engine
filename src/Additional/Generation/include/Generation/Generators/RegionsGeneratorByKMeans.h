#pragma once

#include <Algorithms/ObjectsKMeans.h>
#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapRegion.h>
#include <Tilemap/HexagonalTilemap.h>
#include <core/GameObject.h>
#include <core/Random.h>
#include <core/Transform.h>
#include <manager/PrefabManager.h>

namespace Generation::Generators
{
    class RegionsGeneratorByKMeans : public AMapElementGenerator
    {
        SCRIPTABLE_OBJECT_BODY(RegionsGeneratorByKMeans)

    public:
        //Twin2Engine::Core::GameObject* regionPrefab;
        Twin2Engine::Core::Prefab* regionPrefab;
        int regionsCount = 3; // Number of regions/clusters

        bool isDiscritizedHeight = false;
        float lowerHeightRange = 0.f;
        float upperHeightRange = 0.f;
        float heightRangeFacor = 1.f;

        void Generate(Tilemap::HexagonalTilemap* tilemap) override;

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

            if (regionPrefab != nullptr) {
                if (!prefabNames.contains(regionPrefab->GetId())) {
                    regionPrefab = nullptr;
                }
            }

            size_t prefabId = regionPrefab != nullptr ? regionPrefab->GetId() : 0;

            if (ImGui::BeginCombo(string("regionPrefab##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

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
                        regionPrefab = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                    }
                    else {
                        regionPrefab = nullptr;
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::InputInt(string("regionsCount##SO").append(id).c_str(), &regionsCount);
            ImGui::Checkbox(string("isDiscritizedHeight##SO").append(id).c_str(), &isDiscritizedHeight);
            ImGui::InputFloat(string("lowerHeightRange##SO").append(id).c_str(), &lowerHeightRange);
            ImGui::InputFloat(string("upperHeightRange##SO").append(id).c_str(), &upperHeightRange);
            ImGui::InputFloat(string("heightRangeFacor##SO").append(id).c_str(), &heightRangeFacor);

            ScriptableObject::DrawInheritedFields();
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(RegionsGeneratorByKMeans, Generation::Generators)
