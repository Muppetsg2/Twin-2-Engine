#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>
#include <core/Random.h>

#include <Humans/HumanMovement.h>

#include <Generation/CitiesManager.h>

namespace Humans
{
	class Human : public Twin2Engine::Core::Component
    {

        size_t finishedMovingEventHandlerId;

        float achievingDestinationAccuracity = 0.2f;

        HumanMovement* _movement;

        bool _work = false;

        Twin2Engine::Core::GameObject* _targetCity = nullptr;

        void FinishedMoving(HumanMovement* movement);
    public:
        virtual void Initialize() override;
        virtual void OnDestroy() override;


        void StartWorking();
        void StopWorking();

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
        virtual void DrawEditor() override;
#endif
	};
}