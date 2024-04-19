#pragma once

#include <Algorithms/ObjectsKMeans.h>
#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapRegion.h>
#include <Tilemap/HexagonalTilemap.h>
#include <core/GameObject.h>
#include <core/Random.h>
#include <core/Transform.h>

namespace Generation::Generators
{
    class RegionsGeneratorByKMeans : public AMapElementGenerator
    {
    public:
        Twin2Engine::Core::GameObject* regionPrefab;
        int regionsCount = 3; // Number of regions/clusters

        bool isDiscritizedHeight = false;
        float lowerHeightRange = 0f;
        float upperHeightRange = 0f;
        float heightRangeFacor = 1f;

        void Generate(Tilemap::HexagonalTilemap* tilemap) override;
    };
}
