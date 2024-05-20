#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Prefab.h>
#include <core/Random.h>

#include <manager/PrefabManager.h>

#include <Tilemap/HexagonalTile.h>
#include <Tilemap/HexagonalTilemap.h>

#include <Generation/MapHexTile.h>

#include <Generation/ContentGenerator.h>

#define HEX_DIRECTION_CONNECTING true
#define GRADIENT_CONNECTING false

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
        //Twin2Engine::Core::GameObject* preafabHexagonalTile;
        //Twin2Engine::Core::GameObject* additionalTile;
        //Twin2Engine::Core::GameObject* filledTile;
        //Twin2Engine::Core::GameObject* pointTile;
        Twin2Engine::Core::Prefab* preafabHexagonalTile;
        Twin2Engine::Core::Prefab* additionalTile;
        Twin2Engine::Core::Prefab* filledTile;
        Twin2Engine::Core::Prefab* pointTile;


        //float generationRadius = 5.0f;
        float generationRadiusMin = 5.0f;
        float generationRadiusMax = 5.0f;

        int minPointsNumber = 5;
        int maxPointsNumber = 5;
        float angleDeltaRange = 5.0f;

#pragma endregion
        void Generate();

        virtual void Initialize() override;
        virtual void OnEnable() override;

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;

        virtual void DrawEditor() override;
	};
}