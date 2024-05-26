#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/Time.h>

#include <physic/CollisionManager.h>
#include <physic/Ray.h>

#include <Generation/CitiesManager.h>
#include <Generation/MapHexTile.h>

#include <AstarPathfinding/AStarPathfinder.h>


namespace Humans
{
    class HumanMovement : public Twin2Engine::Core::Component
    {
        float _speed = 1.0f;
        glm::vec3 _targetDestination;
        glm::vec3 _bufferedTargetDestination;
        glm::vec3 _currentDestination;
        Tilemap::HexagonalTilemap* _tilemap;

        float _forwardDetectionDistance = 0.1f;

        float _achievingDestinationAccuracity = 0.2f;
        
        // Start is called before the first frame update

        AStar::AStarPath _path;
        bool _findingPath = false;
        bool _foundPath = false;

        void PathFindingSuccess(const AStar::AStarPath& path);
        void PathFindingFailure();

    public:
        //HumanMovement() : Twin2Engine::Core::Component() { }

        virtual void Initialize() override;

        virtual void Update() override;

        void MoveTo(glm::vec3 destination);

        glm::vec3 GetTargetDestination() const;

        void SetTilemap(Tilemap::HexagonalTilemap* tilemap);

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
        virtual void DrawEditor() override;
#endif
    };
}