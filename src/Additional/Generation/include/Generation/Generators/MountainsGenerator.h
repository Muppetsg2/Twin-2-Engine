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
        SCRIPTABLE_OBJECT_BODY(MountainsGenerator)

    public:
        Twin2Engine::Core::GameObject* prefabMountains;
        int mountainsNumber = 0;

        float mountainsHeight = 0.2f;

        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

        SO_SERIALIZE()
        SO_DESERIALIZE()
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(Generation::Generators::MountainsGenerator)

#endif // !_MOUNTAINS_GENERATOR_H_
