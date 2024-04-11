#ifndef _MAP_GENERATOR_H_
#define _MAP_GENERATOR_H_

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Random.h>

#include <Tilemap/HexagonalTile.h>
#include <Tilemap/HexagonalTilemap.h>

namespace Generation
{
	class MapGenerator : public ::Twin2Engine::Core::Component
	{

        void GenerateFloatHull(const std::vector<glm::vec2>& hull);

        void GeneratePositions(const std::vector<glm::ivec2>& positions);
        
        void GenerateForFloatPositions(const std::vector<glm::vec2>& positions);

        void ConnectTiles(glm::ivec2 startTile, glm::ivec2 endTile);

        void GenerateRandomHull();

    public:
#pragma region COMPONENT_PARAMETERS

        Tilemap::HexagonalTilemap* tilemap;
        //Tilemap::HexagonalTile* tile;
        Twin2Engine::Core::GameObject* preafabHexagonalTile;
        Twin2Engine::Core::GameObject* additionalTile;
        Twin2Engine::Core::GameObject* filledTile;
        Twin2Engine::Core::GameObject* pointTile;


        float generationRadius = 5.0f;
        float generationRadiusMinModifier = 0.0f;
        float generationRadiusMaxModifier = 0.0f;

        int minPointsNumber = 5;
        int maxPointsNumber = 5;
        float angleDeltaRange = 5.0f;

#pragma endregion
        void Generate();


	};
}

#endif // !_MAP_GENERATOR_H_