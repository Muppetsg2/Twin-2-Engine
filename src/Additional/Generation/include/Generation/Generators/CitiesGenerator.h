#pragma once

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
		//Twin2Engine::Core::GameObject* prefabCity;
		Twin2Engine::Core::Prefab* prefabCity = nullptr;

		bool byRegions = true;
		float density = 1.0f;

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

		SO_SERIALIZE()
			SO_DESERIALIZE()

#if _DEBUG
		virtual void DrawEditor() override
		{
			// TODO: Dodac prefab City do edytora
			ImGui::Checkbox("byRegions", &byRegions);
			ImGui::SliderFloat("Density", &density, 0.0f, 1.0f);
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(CitiesGenerator, Generation::Generators)