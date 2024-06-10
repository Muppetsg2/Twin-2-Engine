#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

#include <manager/SceneManager.h>

namespace Generation::Generators
{
	class RegionsBySectorsGenerator : public AMapElementGenerator
    {
        SCRIPTABLE_OBJECT_BODY(RegionsBySectorsGenerator)

    public:
        //Twin2Engine::Core::GameObject* regionPrefab = nullptr;
        Twin2Engine::Core::Prefab* regionPrefab = nullptr;
        bool mergeByNumberTilesPerRegion = false;
        int minTilesPerRegion = 10;
        int maxTilesPerRegion = 20;
        int minSectorsPerRegion = 3;
        int maxSectorsPerRegion = 4;
        bool isDiscritizedHeight = false;
        float lowerHeightRange = 0.0f;
        float upperHeightRange = 0.0f;
        float heightRangeFacor = 1.0f;

        //RegionsBySectorsGenerator() = default;
        //virtual ~RegionsBySectorsGenerator() { }
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

            ImGui::Checkbox(string("mergeByNumberTilesPerRegion##SO").append(id).c_str(), &mergeByNumberTilesPerRegion);
            ImGui::InputInt(string("minTilesPerRegion##SO").append(id).c_str(), &minTilesPerRegion);
            ImGui::InputInt(string("maxTilesPerRegion##SO").append(id).c_str(), &maxTilesPerRegion);
            ImGui::InputInt(string("minSectorsPerRegion##SO").append(id).c_str(), &minSectorsPerRegion);
            ImGui::InputInt(string("maxSectorsPerRegion##SO").append(id).c_str(), &maxSectorsPerRegion);
            ImGui::Checkbox(string("isDiscritizedHeight##SO").append(id).c_str(), &isDiscritizedHeight);
            ImGui::InputFloat(string("upperHeightRange##SO").append(id).c_str(), &upperHeightRange);
            ImGui::InputFloat(string("upperHeightRange##SO").append(id).c_str(), &upperHeightRange);
            ImGui::InputFloat(string("heightRangeFacor##SO").append(id).c_str(), &heightRangeFacor);

            ScriptableObject::DrawInheritedFields();
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(RegionsBySectorsGenerator, Generation::Generators)