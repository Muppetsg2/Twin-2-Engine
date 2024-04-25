#ifndef _RADIO_STATION_GENERATOR_REGION_BASED_H_
#define _RADIO_STATION_GENERATOR_REGION_BASED_H_

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class RadioStationGeneratorRegionBased : public AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(RadioStationGeneratorRegionBased)

    public:
        Twin2Engine::Core::GameObject* prefabRadioStation;
        float densityFactorPerRegion = 1.0f;

        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

		SO_SERIALIZE()
		SO_DESERIALIZE()
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(Generation::Generators::RadioStationGeneratorRegionBased)

#endif // !_RADIO_STATION_GENERATOR_REGION_BASED_H_
