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

namespace Generation::Generators {
    class SectorGeneratorForRegionsByKMeans : public AMapElementGenerator {
    public:
        Twin2Engine::Core::GameObject* sectorPrefab;
        int sectorsCount = 3; // Number of regions/clusters

        bool isDiscritizedHeight = false;
        float lowerHeightRange = 0.0f;
        float upperHeightRange = 0.0f;
        float heightRangeFacor = 1.0f;

        void Generate(Tilemap::HexagonalTilemap* tilemap) override;
    };
}
