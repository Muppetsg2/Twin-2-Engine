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

        SO_SERIALIZE()
        SO_DESERIALIZE()


#if _DEBUG
        virtual void DrawEditor() override
        {
            ImGui::Checkbox("mergeByNumberTilesPerRegion", &mergeByNumberTilesPerRegion);
            ImGui::InputInt("minTilesPerRegion", &minTilesPerRegion);
            ImGui::InputInt("maxTilesPerRegion", &maxTilesPerRegion);
            ImGui::InputInt("minSectorsPerRegion", &minSectorsPerRegion);
            ImGui::InputInt("maxSectorsPerRegion", &maxSectorsPerRegion);
            ImGui::Checkbox("isDiscritizedHeight", &isDiscritizedHeight);
            ImGui::InputFloat("upperHeightRange", &upperHeightRange);
            ImGui::InputFloat("upperHeightRange", &upperHeightRange);
            ImGui::InputFloat("heightRangeFacor", &heightRangeFacor);
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(RegionsBySectorsGenerator, Generation::Generators)