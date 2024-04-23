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
		SCRIPTABLE_OBJECT_BODY(CitiesGenerator)

	public:
		Twin2Engine::Core::GameObject* prefabCity;
		bool byRegions = true;
		float density = 1.0f;

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

		virtual void Serialize(YAML::Node& node) const override;
		virtual bool Deserialize(const YAML::Node& node) override;
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(Generation::Generators::CitiesGenerator)

#endif // !_CITIES_GENERATOR_H_
