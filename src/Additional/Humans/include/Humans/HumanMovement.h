#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/Time.h>

#include <physic/CollisionManager.h>
#include <physic/Ray.h>

#include <Generation/CitiesManager.h>
#include <Generation/MapHexTile.h>


namespace Humans
{
    class HumanMovement : public Twin2Engine::Core::Component
    {
        float _speed = 1.0f;
        glm::vec3 _targetDestination;
        Tilemap::HexagonalTilemap* _tilemap;

        float _forwardDetectionDistance = 0.1f;
        // Start is called before the first frame update

    public:
        virtual void Initialize() override;

        virtual void Update() override;

        void MoveTo(glm::vec3 destination);

        glm::vec3 GetTargetDestination() const;

        void SetTilemap(Tilemap::HexagonalTilemap* tilemap);

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;
        virtual void DrawEditor() override;
    };
}