#pragma once

#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/MapHexTile.h>

#include<AstarPathfinding/AStarPathfindingNode.h>

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
            ImGui::InputInt("mountainsNumber", &mountainsNumber);
            ImGui::InputFloat("mountainsHeight", &mountainsHeight);
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(MountainsGenerator, Generation::Generators)