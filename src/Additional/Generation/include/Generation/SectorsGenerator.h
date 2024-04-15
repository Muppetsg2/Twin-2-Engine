#ifndef _SECTORS_GENERATOR_H_
#define _SECTORS_GENERATOR_H_

#include <Generation/AMapElementGenerator.h>
#include <Generation/MapSector.h>
#include <Generation/VectorShuffler.h>

#include <core/GameObject.h>

namespace Generation
{
	class SectorsGenerator : public AMapElementGenerator
	{
	private:

		MapSector* CreateSector(Tilemap::HexagonalTilemap* tilemap, glm::ivec2 position);

	public:
		Twin2Engine::Core::GameObject* prefabSector;
		int minTilesPerSector = 7;
		int maxTilesPerSctor = 7;

		float accuracyFactor = 1.0f;

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;


	};
}

#endif // !_SECTORS_GENERATOR_H_