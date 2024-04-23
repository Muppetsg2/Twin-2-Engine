#ifndef _MOUNTAINS_GENERATOR_H_
#define _MOUNTAINS_GENERATOR_H_

#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/MapHexTile.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class MountainsGenerator : public AMapElementGenerator
    {
    public:
        Twin2Engine::Core::GameObject* prefabMountains;
        int mountainsNumber = 0;

        float mountainsHeight = 0.2f;

        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;
    };
}

#endif // !_MOUNTAINS_GENERATOR_H_
