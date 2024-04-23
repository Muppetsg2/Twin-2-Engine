#ifndef _RADIO_STATION_GENERATOR_SECTOR_BASED_H_
#define _RADIO_STATION_GENERATOR_SECTOR_BASED_H_

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class RadioStationGeneratorSectorBased : public AMapElementGenerator
	{
	public:
		Twin2Engine::Core::GameObject* prefabRadioStation;
		float densityFactorPerSector = 1.0f;

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;
	};
}

#endif // !_RADIO_STATION_GENERATOR_SECTOR_BASED_H_
