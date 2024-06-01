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

        float achievingDestinationAccuracity = 0.2f;


        HumanMovement* _movement;
        bool _work = false;

        Twin2Engine::Core::GameObject* _targetCity = nullptr;

    public:
        // Start is called before the first frame update
        void Initialize();

        // Update is called once per frame
        void Update();

        void StartWorking();
        void StopWorking();

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
        virtual void DrawEditor() override;
#endif
	};
}