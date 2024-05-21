#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/VectorShuffler.h>


namespace Generation::Generators
{
    class LakeGenerator : public AMapElementGenerator
    {
        SCRIPTABLE_OBJECT_BODY(LakeGenerator)

    public:
        int numberOfLakes = 0;
        float waterLevel = 0.0f;

        bool destroyWaterTile = true;
        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

        SO_SERIALIZE()
        SO_DESERIALIZE()

#if _DEBUG
        virtual void DrawEditor() override
        {
            ImGui::Checkbox("destroyWaterTile", &destroyWaterTile);
            ImGui::InputInt("numberOfLakes", &numberOfLakes);
            ImGui::InputFloat("waterLevel", &waterLevel);
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(LakeGenerator, Generation::Generators)