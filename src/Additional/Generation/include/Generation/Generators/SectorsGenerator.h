#pragma once

#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapSector.h>
#include <Generation/VectorShuffler.h>

#include <core/GameObject.h>
#include <manager/SceneManager.h>

namespace Generation::Generators
{
	class SectorsGenerator : public AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(SectorsGenerator)
	private:

		MapSector* CreateSector(Tilemap::HexagonalTilemap* tilemap, glm::ivec2 position);

	public:
		//Twin2Engine::Core::GameObject* prefabSector = nullptr;
		Twin2Engine::Core::Prefab* prefabSector = nullptr;
		int minTilesPerSector = 7;
		int maxTilesPerSector = 7;

		float accuracyFactor = 1.0f;

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;


		SO_SERIALIZE()
		SO_DESERIALIZE()

#if _DEBUG
		virtual void DrawEditor() override
		{
			ImGui::InputInt("minTilesPerSector", &minTilesPerSector);
			ImGui::InputInt("maxTilesPerSector", &maxTilesPerSector);
			ImGui::InputFloat("accuracyFactor", &accuracyFactor);
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(SectorsGenerator, Generation::Generators)