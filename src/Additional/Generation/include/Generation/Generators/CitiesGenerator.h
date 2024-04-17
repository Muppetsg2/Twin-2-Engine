#ifndef _CITIES_GENERATOR_H_
#define _CITIES_GENERATOR_H_


#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class CitiesGenerator : public AMapElementGenerator
	{
	public:
		Twin2Engine::Core::GameObject* prefabCity;
		bool byRegions = true;
		float density = 1.0f;

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;
	};
}

#endif // !_CITIES_GENERATOR_H_
