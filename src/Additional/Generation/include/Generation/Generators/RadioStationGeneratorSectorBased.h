#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class RadioStationGeneratorSectorBased : public AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(RadioStationGeneratorSectorBased)

	public:
		//Twin2Engine::Core::GameObject* prefabRadioStation;
		Twin2Engine::Core::Prefab* prefabRadioStation;
		float densityFactorPerSector = 1.0f;

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

		SO_SERIALIZE()
		SO_DESERIALIZE()

#if _DEBUG
		virtual void DrawEditor() override
		{
			ImGui::InputFloat("densityFactorPerSector", &densityFactorPerSector);
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(RadioStationGeneratorSectorBased, Generation::Generators)